/*************************************************************************************************
 *                                                                                                *
 *  file: categorytreeitem.h                                                                      *
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

#ifndef CATEGORY_TREE_ITEM_H
#define CATEGORY_TREE_ITEM_H

#include <QVariantList>


class CategoryTreeItem
{
    public:
        explicit CategoryTreeItem( const QVariantList& categoryData, CategoryTreeItem* parentItem = nullptr )
          : categoryData( categoryData ), parentItem( parentItem ) {}

        ~CategoryTreeItem()
        {
            qDeleteAll( this->childItems );
        }

        void appendChildItem( CategoryTreeItem* childItem )
        {
            this->childItems.append( childItem );
        }

        CategoryTreeItem* childItem( int row )
        {
            return( this->childItems.value(row) );
        }

        int childItemsCount() const
        {
            return( this->childItems.count() );
        }

        int columnsCount() const
        {
            return( this->categoryData.count() );
        }

        QVariant data( int column ) const
        {
            if( this->categoryData.count() )
            {
                return( this->categoryData.value( column, QVariant() ) );
            }
            else
            {
                return( QVariant() );
            }
        }

        int row() const
        {
            if( this->parentItem == nullptr )
            {
                return( 0 );
            }
            else
            {
                return(
                    this->parentItem->childItems.indexOf( const_cast<CategoryTreeItem*>( this ) )
                );
            }
        }

        CategoryTreeItem* getParentItem()
        {
            return( this->parentItem );
        }

        enum Column
        {
            Name,
            Description,
            Id,
            ParentCategoryId,
            Type
        };

        enum Type
        {
            All = 0,
            Outcome = 1,
            Income = 2
        };

    private:
        QVariantList categoryData;
        CategoryTreeItem* parentItem;
        QList<CategoryTreeItem*> childItems;
};


#endif // CATEGORY_TREE_ITEM_H
