/*************************************************************************************************
 *                                                                                                *
 *  file: transactionsmodel.h                                                                     *
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

#ifndef TRANSACTIONS_MODEL_H
#define TRANSACTIONS_MODEL_H

#include <QAbstractTableModel>
#include <QStringList>
#include <QSqlDatabase>
#include <QVariantList>

#include "types.h"


class TransactionsModel : public QAbstractTableModel
{
    Q_OBJECT

    public:
        explicit TransactionsModel( QSqlDatabase database );

        bool addRecord( const UniMap& fieldsData, unsigned int* createdId = nullptr );

        int rowCount( const QModelIndex& parent = QModelIndex() ) const override;
        int columnCount( const QModelIndex& parent = QModelIndex() ) const override;

        QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const override;
        QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const override;

        enum Type
        {
            OUTGOING,
            INCOMING,
            INTERNAL
        };

    protected:
        QSqlDatabase database;
        mutable QMap<int,QVariantList> records;

        static QStringList titles;
};


#endif // TRANSACTIONS_MODEL_H
