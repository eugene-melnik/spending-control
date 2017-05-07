/*************************************************************************************************
 *                                                                                                *
 *  file: categoriesmodelproxy.h                                                                  *
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

#ifndef CATEGORIES_MODEL_PROXY_H
#define CATEGORIES_MODEL_PROXY_H

#include <QSortFilterProxyModel>
#include <QVariant>

#include "categoriesmodel.h"


class CategoriesModelProxy : public QSortFilterProxyModel
{
    Q_OBJECT

    public:
        explicit CategoriesModelProxy( QObject* parent = nullptr )
            : QSortFilterProxyModel( parent ) {}

        /**
         * \brief   If set only one filter for one column, 'equals' condition will be used
         */
        void setFilter( CategoryTreeItem::Column column, const QVariant& value )
        {
            this->filterValues.insert( column, value );
        }

        /**
         * \brief   If set several filters for one column, 'contains at least one of' condition will be used
         */
        void addFilter( CategoryTreeItem::Column column, const QVariant& value )
        {
            this->filterValues.insertMulti( column, value );
        }

        /**
         * \brief   Remove filters for specified column
         */
        void removeFilter( CategoryTreeItem::Column column )
        {
            this->filterValues.remove( column );
        }

        /**
         * \brief   Remove all the filters
         */
        void clearFilters()
        {
            this->filterValues.clear();
        }

    protected:
        bool filterAcceptsRow( int sourceRow, const QModelIndex& sourceParent ) const override
        {
            bool result = true;

            const CategoryTreeItem* category = static_cast<CategoryTreeItem*>(
                this->sourceModel()->index( sourceRow, 0, sourceParent ).internalPointer()
            );

            for( CategoryTreeItem::Column column : this->filterValues.keys() )
            {
                QList<QVariant> values = this->filterValues.values( column );
                QVariant columnValue = category->data( column );

                if( values.count() == 1 )
                {
                    // Only one filter set
                    result = result && (columnValue == values.first());
                }
                else
                {
                    // Several filters set
                    bool subResult = false;

                    for( const QVariant& value : values )
                    {
                        /** TODO: or 'equals at least one of' ? */
                        subResult = subResult || columnValue.toString().contains( value.toString() );
                    }

                    result = result && subResult;
                }
            }

            return( result );
        }

        QMap<CategoryTreeItem::Column,QVariant> filterValues; /** TODO: move to some AbstractFilterProxy ? */
};


#endif // CATEGORIES_MODEL_PROXY_H
