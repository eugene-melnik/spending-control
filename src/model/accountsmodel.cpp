/*************************************************************************************************
 *                                                                                                *
 *  file: accountsmodel.cpp                                                                       *
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
#include "accountsmodel.h"
#include "defines.h"


QStringList AccountsModel::types = {
    QObject::tr( "<no type>" ),
    QObject::tr( "Bank" ),
    QObject::tr( "Cash" ),
    QObject::tr( "Credit card" )
};

QMap<QString,QVariantList> AccountsModel::currencies = {
    // ISO Code, Territory,     Currency,               Symbol, Fractional unit,    Number to basic
    { "UAH", { "Ukraine",       "Ukrainian hryvnia",    "₴",    "Kopiyka",          100 } },
    { "USD", { "United States", "United States dollar", "$",    "Cent",             100 } }
};


AccountsModel::AccountsModel( QSqlDatabase database )
  : database( database )
{
    this->titles = {
        tr( "ID"),
        tr( "Name" ),
        tr( "Description" ),
        tr( "Type" ),
        tr( "Currency" ),
        tr( "Initial balance" ),
        tr( "Minimal balance" ),
        tr( "Closed at" )
    };
}


bool AccountsModel::addAccountRecord( const UniMap& fieldsData )
{
    this->beginResetModel();

    DatabaseQuery query( this->database );

    query.prepare(
        "INSERT INTO accounts (name, description, type, currency, initial_balance, minimal_balance, created_at) \
        VALUES (?, ?, ?, ?, ?, ?, ?);"
    );

    query.bindValue( 0, fieldsData.value( "name" ) );
    query.bindValue( 1, fieldsData.value( "description" ) );
    query.bindValue( 2, fieldsData.value( "type" ) );
    query.bindValue( 3, fieldsData.value( "currency" ) );
    query.bindValue( 4, (int) fieldsData.value( "initial_balance" ).toDouble() * 100 );
    query.bindValue( 5, (int) fieldsData.value( "minimal_balance" ).toDouble() * 100 );
    query.bindValue( 6, QDateTime::currentDateTime().toString( Qt::ISODate ) );

    bool ok = query.exec();

    if( !ok )
    {
        AppLogger->error( "AccountsModel::addAccountRecord - Can't save account: " + query.lastError().text() );
    }

    this->endResetModel();
    this->records.clear();

    return( ok );
}


bool AccountsModel::updateAccountRecord( const UniMap& fieldsData )
{
    this->beginResetModel();

    DatabaseQuery query( this->database );

    query.prepare(
        "UPDATE accounts SET name = ?, description = ?, type = ?, currency = ?, initial_balance = ?, minimal_balance = ? \
        WHERE id = ?;"
    );

    query.bindValue( 0, fieldsData.value( "name" ) );
    query.bindValue( 1, fieldsData.value( "description" ) );
    query.bindValue( 2, fieldsData.value( "type" ) );
    query.bindValue( 3, fieldsData.value( "currency" ) );
    query.bindValue( 4, (int) fieldsData.value( "initial_balance" ).toDouble() * 100 );
    query.bindValue( 5, (int) fieldsData.value( "minimal_balance" ).toDouble() * 100 );
    query.bindValue( 6, fieldsData.value( "id" ).toInt() );

    bool ok = query.exec();

    if( !ok )
    {
        AppLogger->error( "AccountsModel::updateAccountRecord - Can't save account: " + query.lastError().text() );
    }

    this->endResetModel();
    this->records.clear();

    return( ok );
}


bool AccountsModel::deleteOrCloseAccountRecord( int accountId )
{
    DatabaseQuery query( this->database );

    query.prepare(
        "SELECT COUNT(*) FROM transactions \
        WHERE source_account_id = ? OR destination_account_id = ?;"
    );

    query.bindValue( 0, accountId );
    query.bindValue( 1, accountId );

    if( query.exec() && query.first() && query.value( 0 ).toInt() > 0 )
    {
        return( this->closeAccountRecord( accountId ) );
    }
    else
    {
        return( this->deleteAccountRecord( accountId ) );
    }
}


bool AccountsModel::deleteAccountRecord( int accountId )
{
    this->beginResetModel();

    DatabaseQuery query( this->database );
    query.prepare( "DELETE FROM accounts WHERE id = ?;" );
    query.bindValue( 0, accountId );

    bool ok = query.exec();

    if( !ok )
    {
        AppLogger->error( "AccountsModel::deleteAccountRecord - Can't delete account: " + query.lastError().text() );
    }

    this->endResetModel();
    this->records.clear();

    return( ok );
}


bool AccountsModel::closeAccountRecord( int accountId )
{
    this->beginResetModel();

    DatabaseQuery query( this->database );
    query.prepare( "UPDATE accounts SET closed_at = ? WHERE id = ?;" );
    query.bindValue( 0, QDateTime::currentDateTime().toString( Qt::ISODate ) );
    query.bindValue( 1, accountId );

    bool ok = query.exec();

    if( !ok )
    {
        AppLogger->error( "AccountsModel::closeAccountRecord - Can't delete account: " + query.lastError().text() );
    }

    this->endResetModel();
    this->records.clear();

    return( ok );
}


int AccountsModel::rowCount( const QModelIndex& ) const
{
    QSqlQuery query = this->database.exec(
        "SELECT COUNT(*) AS accounts_count \
        FROM accounts \
        WHERE closed_at IS NULL;"
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


int AccountsModel::columnCount( const QModelIndex& ) const
{
    return( this->titles.count() );
}


QVariant AccountsModel::data( const QModelIndex& index, int role ) const
{
    switch( role )
    {
        case Qt::DisplayRole :
        {
            return( this->getRecord( index.row() ).at( index.column() ) );
        }
    }

    return( QVariant() );
}


QVariant AccountsModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if( orientation == Qt::Horizontal )
    {
        switch( role )
        {
            case Qt::DisplayRole :
            case Qt::ToolTipRole :
            {
                return( this->titles.at( section ) );
            }
        }
    }

    return( QVariant() );
}


QVariantList AccountsModel::getRecord( int row ) const
{
    if( this->records.value( row ).empty() )
    {
        QSqlQuery query( this->database );

        query.prepare(
            "SELECT id, name, description, type, currency, initial_balance, minimal_balance, closed_at \
            FROM accounts WHERE closed_at IS NULL \
            ORDER BY name ASC LIMIT 1 OFFSET ?;"
        );

        query.bindValue( 0, row );
        query.exec();

        if( query.first() )
        {
            this->records.insert( row, {
                query.value( 0 ),
                query.value( 1 ),
                query.value( 2 ),
                query.value( 3 ),
                query.value( 4 ),
                query.value( 5 ),
                query.value( 6 ),
                query.value( 7 )
            } );
        }
        else
        {
            return( QVariantList() );
        }
    }

    return( this->records.value( row ) );
}


UniMap AccountsModel::getRecordsMapped( int row ) const
{
    UniMap result;

    QVariantList record = this->getRecord( row );

    if( !record.isEmpty() )
    {
        result = {
            { "id",             record.at( 0 ) },
            { "name",           record.at( 1 ) },
            { "description",    record.at( 2 ) },
            { "type",           record.at( 3 ) },
            { "currency",       record.at( 4 ) },
            { "initial_balance", record.at( 5 ) },
            { "minimal_balance", record.at( 6 ) },
            { "closed_at",      record.at( 7 ) }
        };
    }

    return( result );
}


UniMap AccountsModel::getList() const
{
    UniMap list;

    for( int row = 0; row < this->rowCount(); ++row )
    {
        QVariantList rowData = this->getRecord( row );
        list.insert( rowData.at( Column::Id ).toString(), rowData.at( Column::Name ) );
    }

    return( list );
}


QStringList AccountsModel::getTypes()
{
    return( AccountsModel::types );
}


QStringList AccountsModel::getCurrencies()
{
    QStringList result;

    for ( const QString& key : AccountsModel::currencies.keys() )
    {
        if( key != "-NONE-" )
        {
            QVariantList values = AccountsModel::currencies.value( key );
            QString name = values.at( 1 ).toString();
            QString symbol = values.at( 2 ).toString();

            result.append( key + " - " + name + " (" + symbol + ")" );
        }
        else
        {
            result.append( key );
        }
    }

    return( result );
}
