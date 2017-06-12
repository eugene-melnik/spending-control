/*************************************************************************************************
 *                                                                                                *
 *  file: accountsmodel.h                                                                         *
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

#ifndef ACCOUNTS_MODEL_H
#define ACCOUNTS_MODEL_H

#include <QAbstractTableModel>
#include <QStringList>
#include <QSqlDatabase>
#include <QVariantList>

#include "types.h"


class AccountsModel : public QAbstractTableModel
{
    Q_OBJECT

    public:
        AccountsModel( QSqlDatabase database );

        bool addRecord( const UniMap& fieldsData );
        bool updateRecord( const UniMap& fieldsData );
        bool deleteRecord( unsigned int accountId );

        bool deleteOrCloseAccount( unsigned int accountId );
        bool closeAccount( unsigned int accountId );

        int rowCount( const QModelIndex& parent = QModelIndex() ) const override;
        int columnCount( const QModelIndex& parent = QModelIndex() ) const override;

        QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const override;
        QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const override;

        QVariantList getRecord( int row ) const;
        UniMap getRecordMapped( int row ) const;

        UniMap getList() const;

        enum Column
        {
            Id,
            Name,
            Description,
            Type,
            Currency,
            InitialBalance,
            MinimalBalance,
            ClosedAt
        };

        static UniMap getById( int id, QSqlDatabase database );

        static QStringList getTypes();
        static UniMap getCurrencies();

    protected:
        QSqlDatabase database;
        mutable QMap<int,QVariantList> records;

        static QStringList titles;
        static QStringList types;
        static QMap<QString,QVariantList> currencies; // TODO: move to the separate class
};


#endif // ACCOUNTS_MODEL_H
