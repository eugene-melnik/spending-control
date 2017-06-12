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
        "INSERT INTO transactions "
            "(type, amount, balance_after, date, planned, source_account_id, destination_account_id, category_id, notes) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);"
    );

    query.bindValue( 0, fieldsData.value( "type" ) );
    query.bindValue( 1, fieldsData.value( "amount" ) );
    query.bindValue( 2, fieldsData.value( "balance_after", 0 ) );
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


bool TransactionsModel::updateRecord( const UniMap& fieldsData )
{
    if( fieldsData.value( "id" ).toUInt() == 0 )
    {
        AppLogger->warn( "Can't updaterecord with id = 0!" );
        return( false );
    }

    DatabaseQuery query( this->database );
    this->beginResetModel();

    query.prepare(
        "UPDATE transactions "
            "SET type = ?, amount = ?, balance_after = ?, date = ?, planned = ?, source_account_id = ?, "
                "destination_account_id = ?, category_id = ?, notes = ? "
        "WHERE id = ?"
    );

    query.bindValue( 0, fieldsData.value( "type" ) );
    query.bindValue( 1, fieldsData.value( "amount" ) );
    query.bindValue( 2, fieldsData.value( "balance_after", 0 ) );
    query.bindValue( 3, fieldsData.value( "date" ) );
    query.bindValue( 4, fieldsData.value( "planned", 0 ) );
    query.bindValue( 5, fieldsData.value( "source_account_id", 0 ) );
    query.bindValue( 6, fieldsData.value( "destination_account_id", 0 ) );
    query.bindValue( 7, fieldsData.value( "category_id" ) );
    query.bindValue( 8, fieldsData.value( "notes", "" ) );
    query.bindValue( 9, fieldsData.value( "id" ) );

    bool ok = query.exec();

    if( !ok )
    {
        AppLogger->error( "TransactionsModel::updateAccountRecord - Can't save transaction: " + query.lastError().text() );
    }

    this->records.clear(); // TODO: optimise this, not necessary to clear all if only one record changed
    this->endResetModel();

    return( ok );
}


bool TransactionsModel::deleteRecord( unsigned int transactionId )
{
    this->beginResetModel();

    DatabaseQuery query( this->database );
    query.prepare( "DELETE FROM transactions WHERE id = ?" );
    query.bindValue( 0, transactionId );

    bool ok = query.exec();

    if( !ok )
    {
        AppLogger->error( "TransactionsModel::deleteAccountRecord - Can't delete transaction: " + query.lastError().text() );
    }

    this->endResetModel();
    this->records.clear();

    return( ok );
}


UniMap TransactionsModel::getRecordForEdit( unsigned int transactionId ) const
{
    DatabaseQuery query( this->database );

    query.prepare(
        "SELECT type, amount, balance_after, date, planned, source_account_id, destination_account_id, category_id, notes "
        "FROM transactions "
        "WHERE id = ?"
    );

    query.bindValue( 0, transactionId );

    if( !query.exec() )
    {
        AppLogger->error( "TransactionsModel::getRecordForEdit - Can't load transaction: " + query.lastError().text() );
    }

    if( query.first() )
    {
        return( UniMap({
            { "id",                 transactionId },
            { "type",               query.value( 0 ) },
            { "amount",             query.value( 1 ) },
            { "balance_after",      query.value( 2 ) },
            { "date",               query.value( 3 ) },
            { "planned",            query.value( 4 ) },
            { "source_account_id",  query.value( 5 ) },
            { "destination_account_id", query.value( 6 ) },
            { "category_id",        query.value( 7 ) },
            { "notes",              query.value( 8 ) }
        }) );
    }
    else
    {
        return( UniMap() );
    }
}


int TransactionsModel::rowCount( const QModelIndex& ) const
{
    QSqlQuery query = this->database.exec(
        "SELECT COUNT(*) AS transactions_count FROM transactions"
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


QVariantList TransactionsModel::getRecord( int row ) const
{
    if( this->records.value( row ).empty() )
    {
        QSqlQuery query( this->database );

        query.prepare(
            "SELECT id, type, amount, balance_after, date, planned, source_account_id, "
                    "destination_account_id, category_id, notes  "
            "FROM transactions ORDER BY date ASC LIMIT ? OFFSET ?"
        );

        query.bindValue( 0, 1 );
        query.bindValue( 1, row );
        query.exec();

        if( query.first() )
        {
            this->records.insert( row, {
                query.value( Column::Id ),
                query.value( Column::Type ),
                query.value( Column::Amount ),
                query.value( Column::BalanceAfter ),
                query.value( Column::Date ),
                query.value( Column::Planned ),
                query.value( Column::SourceAccountId ),
                query.value( Column::DestinationAccountId ),
                query.value( Column::CategoryId ),
                query.value( Column::Notes )
            } );
        }
        else
        {
            return( QVariantList() );
        }
    }

    return( this->records.value( row ) );
}


UniMap TransactionsModel::getRecordMapped( int row ) const
{
    UniMap result;
    QVariantList record = this->getRecord( row );

    if( !record.isEmpty() )
    {
        result = {
            { "id",                 record.at( Column::Id ) },
            { "type",               record.at( Column::Type ) },
            { "amount",             record.at( Column::Amount ) },
            { "balance_after",      record.at( Column::BalanceAfter ) },
            { "date",               record.at( Column::Date ) },
            { "planned",            record.at( Column::Planned ) },
            { "source_account_id",  record.at( Column::SourceAccountId ) },
            { "destination_account_id", record.at( Column::DestinationAccountId ) },
            { "category_id",        record.at( Column::CategoryId ) },
            { "notes",              record.at( Column::Notes ) }
        };
    }

    return( result );
}
