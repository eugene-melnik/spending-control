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


CategoriesModel::CategoriesModel( QSqlDatabase database )
    : database( database )
{
    //
}


int CategoriesModel::rowCount() const
{
    QSqlQuery query = this->database.exec(
        "SELECT COUNT(*) AS categories_count FROM categories WHERE deleted_at IS NULL;"
    );

    if( query.first() )
    {
        return( query.value( 0 ).toInt() );
    }
    else
    {
        return( 0 );
    }
}


QVariantList CategoriesModel::getRecord( int row ) const
{
    if( this->records.value( row ).empty() )
    {
        QSqlQuery query( this->database );

        query.prepare(
            "SELECT id, name, description, parent_category_id, system \
            FROM categories WHERE deleted_at IS NULL ORDER BY id ASC LIMIT ? OFFSET ?;"
        );

        query.bindValue( 0, 1 );
        query.bindValue( 1, row );
        query.exec();

        if( query.first() )
        {
            this->records.insert( row, {
                query.value( Column::Id ),
                query.value( Column::Name ),
                query.value( Column::Description ),
                query.value( Column::ParentCategoryId ),
                query.value( Column::System )
            } );
        }
        else
        {
            return( QVariantList() );
        }
    }

    return( this->records.value( row ) );
}


UniMap CategoriesModel::getList() const
{
    UniMap list;

    for( int row = 0; row < this->rowCount(); ++row )
    {
        QVariantList rowData = this->getRecord( row );
        list.insert( rowData.at( Column::Id ).toString(), rowData.at( Column::Name ) );
    }

    return( list );
}
