/*************************************************************************************************
 *                                                                                                *
 *  file: transactionsmodel.cpp                                                                   *
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

#include "database/query.h"
#include "defines.h"
#include "transactionsmodel.h"


QStringList TransactionsModel::titles = {
    tr( "ID" )
};


TransactionsModel::TransactionsModel( QSqlDatabase database )
  : database( database )
{
    //
}


bool TransactionsModel::addRecord( const UniMap& fieldsData, unsigned int* createdId )
{
    this->beginResetModel();

    DatabaseQuery query( this->database );

    query.prepare(
        "INSERT INTO transactions \
            (type, amount, balance_after, date, planned, source_account_id, destination_account_id, category_id, notes) \
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);"
    );

    query.bindValue( 0, fieldsData.value( "type" ) );
    query.bindValue( 1, (int) fieldsData.value( "amount" ).toDouble() * 100 );
    query.bindValue( 2, (int) fieldsData.value( "balance_after" ).toDouble() * 100 );
    query.bindValue( 3, fieldsData.value( "date" ) );
    query.bindValue( 4, fieldsData.value( "planned", 0 ) );
    query.bindValue( 5, fieldsData.value( "source_account_id", 0 ) );
    query.bindValue( 6, fieldsData.value( "destination_account_id", 0 ) );
    query.bindValue( 7, fieldsData.value( "category_id" ) );
    query.bindValue( 8, fieldsData.value( "notes", "" ) );

    bool ok = query.exec();

    if( !ok )
    {
        AppLogger->error( "TransactionsModel::addTransactionRecord - Can't save transaction: " + query.lastError().text() );
    }

    if( createdId )
    {
        if( query.lastInsertId().isValid() )
        {
            *createdId = query.lastInsertId().toInt();
        }
        else
        {
            AppLogger->error( "TransactionsModel::addTransactionRecord - Can't retrieve last inserted record id" );
        }
    }

    this->endResetModel();
    this->records.clear();

    return( ok );
}


int TransactionsModel::rowCount( const QModelIndex& ) const
{
    return( 0 ); // FIXME: dummy
}


int TransactionsModel::columnCount( const QModelIndex& ) const
{
    return( TransactionsModel::titles.count() );
}


QVariant TransactionsModel::data( const QModelIndex& index, int role ) const
{
    return( QVariant() ); // FIXME: dummy
}


QVariant TransactionsModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if( orientation == Qt::Horizontal )
    {
        switch( role )
        {
            case Qt::DisplayRole :
            case Qt::ToolTipRole :
            {
                return( TransactionsModel::titles.at( section ) );
            }
        }
    }

    return( QVariant() );
}
