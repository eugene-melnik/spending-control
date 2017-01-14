/*************************************************************************************************
 *                                                                                                *
 *  file: migration.cpp                                                                           *
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

#include "migration.h"
#include "query.h"

#include <QSqlError>


DatabaseMigration::DatabaseMigration( QSqlDatabase database )
{
    this->database = database;
    this->setupMigrations();
}


int DatabaseMigration::getCurrentVersion() const
{
    int version = 0;

    DatabaseQuery query( this->database );
    query.exec( "SELECT value FROM service_options WHERE name = 'migration_version'" );

    if( query.first() )
    {
        version = query.value( 0 ).toInt();
    }

    return( version );
}


bool DatabaseMigration::migrateToVersion( int version )
{
    int currentVersion = this->getCurrentVersion();

    if( currentVersion == version )
    {
        return( true );
    }

    this->error.clear();
    this->database.transaction();

    while( currentVersion < version )
    {
        QStringList migrations = this->migrationsList.value( currentVersion + 1 );

        for( const QString& migration : migrations )
        {
            QSqlQuery query = this->database.exec( migration );

            if( query.lastError().isValid() )
            {
                this->database.rollback();
                this->error = query.lastError().text();
                return( false );
            }
        }

        ++currentVersion;
    };

    if( !this->updateMigrationVersion( version ) )
    {
        this->error = "Can't update migration version - " + this->error;
        this->database.rollback();
        return( false );
    }

    this->database.commit();

    return( true );
}


QString DatabaseMigration::getLastError() const
{
    return( this->error );
}


void DatabaseMigration::setupMigrations()
{
    /*
     * Create empty 'service' table and add initial 'migration_version' value.
     */
    this->migrationsList.insert(
        1,
        {
            "PRAGMA encoding = 'UTF-8';",

            "CREATE TABLE `service_options` ( \
                `name`              TEXT NOT NULL, \
                `value`             TEXT, \
                PRIMARY KEY(name) \
            );",

            "INSERT INTO service_options (name, value) VALUES ('migration_version', 0);"
        }
    );

    /*
     * Initial versions of tables 'accounts', 'transactions'.
     */
    this->migrationsList.insert(
        2,
        {
            "CREATE TABLE `accounts` ( \
                `id`                INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, \
                `name`              TEXT NOT NULL, \
                `description`       TEXT NOT NULL, \
                `type`              INTEGER NOT NULL DEFAULT '0', \
                `initial_balance`   INTEGER DEFAULT '0', \
                `minimal_balance`   INTEGER DEFAULT '0' \
            );",
            "CREATE INDEX accounts_name_idx ON accounts(name);",
            "CREATE INDEX accounts_type_idx ON accounts(type);",

            "CREATE TABLE `categories` ( \
                `id`                INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, \
                `name`              TEXT NOT NULL, \
                `description`       TEXT NOT NULL, \
                `parent_category_id` INTEGER \
            );",

            "CREATE TABLE `transactions` ( \
                `id`                INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, \
                `type`              INTEGER NOT NULL DEFAULT '0', \
                `amount`            INTEGER NOT NULL DEFAULT '0', \
                `timestamp`         TEXT NOT NULL, \
                `source_account_id` INTEGER NOT NULL, \
                `category_id`       INTEGER NOT NULL, \
                `notes`             TEXT \
            );",
            "CREATE INDEX transactions_type_idx ON transactions(type);",
            "CREATE INDEX transactions_timestamp_idx ON transactions(timestamp);",
            "CREATE INDEX transactions_category_id_idx ON transactions(category_id);"
        }
    );
}


bool DatabaseMigration::updateMigrationVersion( int version )
{
    DatabaseQuery query( this->database );
    query.prepare( "UPDATE service_options SET value = ? WHERE name = 'migration_version';" );
    query.bindValue( 0, version );

    if( query.exec() )
    {
        return( true );
    }
    else
    {
        this->error = query.lastError().text();
        return( false );
    }
}