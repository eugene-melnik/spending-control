/*************************************************************************************************
 *                                                                                                *
 *  file: categoriesmodel.cpp                                                                     *
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

#include "categoriesmodel.h"


const QMap<int,QString> CategoriesModel::typesMap = {
    { CategoryTreeItem::Type::All,      tr( "All" ) },
    { CategoryTreeItem::Type::Outcome,  tr( "Outcome" ) },
    { CategoryTreeItem::Type::Income,   tr( "Income" ) },
};


CategoriesModel::CategoriesModel( QSqlDatabase database, QObject* parent )
  : QAbstractItemModel( parent ),
    database( database )
{
    this->loadAllData();
}


CategoriesModel::~CategoriesModel()
{
    if( this->rootItem != nullptr )
    {
        delete this->rootItem;
    }
}


QVariant CategoriesModel::data( const QModelIndex& index, int role ) const
{
    if( index.isValid() && role == Qt::DisplayRole )
    {
        CategoryTreeItem* item = static_cast<CategoryTreeItem*>( index.internalPointer() );

        switch( index.column() )
        {
            case CategoryTreeItem::Column::Type :
            {
                int type = item->data( index.column() ).toInt();

                if( CategoriesModel::typesMap.contains( type ) )
                {
                    return( CategoriesModel::typesMap.value( type ) );
                }
            }

            default :
            {
                return( item->data( index.column() ) );
            }
        }
    }
    else
    {
        return( QVariant() );
    }
}


Qt::ItemFlags CategoriesModel::flags( const QModelIndex& index ) const
{
    if( index.isValid() )
    {
        return( QAbstractItemModel::flags( index ) );
    }
    else
    {
        return( 0 );
    }
}


QModelIndex CategoriesModel::parent( const QModelIndex& index ) const
{
    if( index.isValid() )
    {
        CategoryTreeItem* childItem = static_cast<CategoryTreeItem*>( index.internalPointer() );
        CategoryTreeItem* parentItem = childItem->getParentItem();

        if( parentItem != this->rootItem )
        {
            return this->createIndex( parentItem->row(), 0, parentItem );
        }
    }

    return QModelIndex();
}


QModelIndex CategoriesModel::index( int row, int column, const QModelIndex& parent ) const
{
    if( this->hasIndex(row, column, parent) )
    {
        CategoryTreeItem* parentItem;

        if( parent.isValid() )
        {
            parentItem = static_cast<CategoryTreeItem*>( parent.internalPointer() );
        }
        else
        {
            parentItem = this->rootItem;
        }

        CategoryTreeItem* childItem = parentItem->childItem( row );

        if( childItem )
        {
            return( createIndex( row, column, childItem ) );
        }
    }

    return( QModelIndex() );
}


int CategoriesModel::rowCount( const QModelIndex& parent ) const
{
    CategoryTreeItem* parentItem;

    if( parent.column() <= 0 )
    {
        if( parent.isValid() )
        {
            parentItem = static_cast<CategoryTreeItem*>( parent.internalPointer() );
        }
        else
        {
            parentItem = this->rootItem;
        }

        return( parentItem->childItemsCount() );
    }

    return( 0 );
}


int CategoriesModel::columnCount( const QModelIndex& parent ) const
{
    CategoryTreeItem* item;

    if( parent.isValid() )
    {
        item = static_cast<CategoryTreeItem*>( parent.internalPointer() );
    }
    else
    {
        item = this->rootItem;
    }

    return( item->columnsCount() );
}


void CategoriesModel::loadAllData()
{
    QVariantList rootItemData = this->getRootCategory();

    if( !rootItemData.isEmpty() )
    {
        // root category added twice, as root item is not shown in tree view,
        // but we need it
        this->rootItem = new CategoryTreeItem( rootItemData );

        CategoryTreeItem* mainItem = new CategoryTreeItem( rootItemData, this->rootItem );
        this->rootItem->appendChildItem( mainItem );

        this->loadSubscategories( mainItem );
    }
}


void CategoriesModel::loadSubscategories( CategoryTreeItem* parentCategory )
{
    int parentCategoryId = parentCategory->data( CategoryTreeItem::Column::Id ).toInt();
    QList<QVariantList> foundSubcategories = this->getSubcategories( parentCategoryId );

    for( const QVariantList& categoryData : foundSubcategories )
    {
        CategoryTreeItem* categoryItem = new CategoryTreeItem( categoryData, parentCategory );
        parentCategory->appendChildItem( categoryItem );
        this->loadSubscategories( categoryItem );
    }
}


QVariantList CategoriesModel::getRootCategory()
{
    QList<QVariantList> foundRecords = this->getSubcategories( 0 );

    if( !foundRecords.isEmpty() )
    {
        return( this->getSubcategories( 0 ).first() );
    }
    else
    {
        return( QVariantList() );
    }
}


QList<QVariantList> CategoriesModel::getSubcategories( int parentCategoryId )
{
    QList<QVariantList> foundRecords;
    QSqlQuery query( this->database );

    if( parentCategoryId == 0 )
    {
        query.prepare(
            "SELECT name, description, id, parent_category_id, type "
            "FROM categories "
            "WHERE deleted_at IS NULL AND parent_category_id IS NULL"
        );
    }
    else
    {
        query.prepare(
            "SELECT name, description, id, parent_category_id, type "
            "FROM categories "
            "WHERE deleted_at IS NULL AND parent_category_id = ?"
        );

        query.bindValue( 0, parentCategoryId );
    }

    if( query.exec() )
    {
        while( query.next() )
        {
            foundRecords.append( {
                query.value( CategoryTreeItem::Column::Name ),
                query.value( CategoryTreeItem::Column::Description ),
                query.value( CategoryTreeItem::Column::Id ),
                query.value( CategoryTreeItem::Column::ParentCategoryId ),
                query.value( CategoryTreeItem::Column::Type )
            } );
        }
    }

    return( foundRecords );
}
