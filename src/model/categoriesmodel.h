/*************************************************************************************************
 *                                                                                                *
 *  file: categoriesmodel.h                                                                       *
 *                                                                                                *
 *  SpendingControl                                                                               *
 *  Copyright (C) 2017 Eugene Melnik <jeka7js@gmail.com>                                          *
 *                                                                                                *
 *  SpendingControl is free software; you can redistribute it and/or modify it under the terms of *
 *  the GNU General Public License as published by the Free Software Foundation; either version 2 *
 *  of the License, or (at your option) any later version.                                        *
 *                                                                                                *
 *  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;     *
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.     *
 *  See the GNU General Public License for more details.                                          *
 *                                                                                                *
 *  You should have received a copy of the GNU General Public License along with this program.    *
 *  If not, see <http://www.gnu.org/licenses/>.                                                   *
 *                                                                                                *
  *************************************************************************************************/

#ifndef CATEGORIES_MODEL_H
#define CATEGORIES_MODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QMap>

#include "categorytreeitem.h"
#include "database/query.h"
#include "types.h"


class CategoriesModel : public QAbstractItemModel
{
    Q_OBJECT

    public:
        explicit CategoriesModel( QSqlDatabase database, QObject* parent = nullptr );
        ~CategoriesModel();

        QVariant data( const QModelIndex& index, int role ) const override;
        Qt::ItemFlags flags( const QModelIndex& index ) const override;

        QModelIndex parent( const QModelIndex& index ) const override;
        QModelIndex index( int row, int column, const QModelIndex& parent = QModelIndex() ) const override;

        int rowCount( const QModelIndex& parent = QModelIndex() ) const override;
        int columnCount( const QModelIndex& parent = QModelIndex() ) const override;

    protected:
        void loadAllData();
        void loadSubscategories( CategoryTreeItem* parentCategory );
        QVariantList getRootCategory();
        QList<QVariantList> getSubcategories( int parentCategoryId );

    private:
        QSqlDatabase database;
        CategoryTreeItem* rootItem = nullptr;

        static const QMap<int,QString> typesMap;
};


#endif // CATEGORIES_MODEL_H
