
#include "accountsmodel.h"

#include <QSqlQuery>


AccountsModel::AccountsModel( QSqlDatabase database )
  : database( database )
{
    this->titles = {
        tr( "Name" ),
        tr( "Description" ),
        tr( "Type" ),
        tr( "Initial balance" ),
        tr( "Minimal balance" ),
        tr( "Closed at" )
    };
}


int AccountsModel::rowCount( const QModelIndex& ) const
{
    QSqlQuery query = this->database.exec(
        "SELECT COUNT(*) AS accounts_count \
        FROM accounts \
        WHERE deleted_at IS NULL;"
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
            "SELECT name, description, type, initial_balance, minimal_balance, closed_at \
            FROM accounts \
            WHERE deleted_at IS NULL \
            ORDER BY created_at DESC \
            LIMIT 1 OFFSET ?;"
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
                query.value( 5 )
            } );
        }
        else
        {
            return( QVariantList() ); // TODO: ?
        }
    }

    return( this->records.value( row ) );
}
