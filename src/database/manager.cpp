/*************************************************************************************************
 *                                                                                                *
 *  file: manager.h                                                                               *
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

#include "manager.h"

#include <QFile>


QString DatabaseManager::DriverNames[] = {
    "QDB2",       // IBM DB2 (version 7.1 and above)
    "QIBASE",     // Borland InterBase
    "QMYSQL",     // MySQL
    "QOCI",       // Oracle Call Interface Driver
    "QODBC",      // Open Database Connectivity (ODBC) - Microsoft SQL Server and other ODBC-compliant databases
    "QPSQL",      // PostgreSQL (versions 7.3 and above)
    "QSQLITE2",   // SQLite version 2
    "QSQLITE",    // SQLite version 3
    "QSYMSQL",    // SQLite version 3 for Symbian SQL Database
    "QTDS"        // Sybase Adaptive Server. Note: obsolete from Qt 4.7
};

DatabaseManager* DatabaseManager::instance = nullptr;


std::unique_ptr<QSqlError> DatabaseManager::setupInstance( UniMap options )
{
    std::unique_ptr<QSqlError> error;

    if( DatabaseManager::instance != nullptr )
    {
        DatabaseManager::clearInstance();
    }

    if( !options.contains( "create_non_exist" ) )
    {
        options.insert( "create_non_exist", true );
    }

    if( !options.contains( "driver_name" ) )
    {
        options.insert( "driver_name", DriverNames[ defaultDriver ] );
    }

    if( options.value( "create_non_exist" ).toBool() || QFile::exists( options.value( "database_name" ).toString() ) )
    {
        DatabaseManager::instance = new DatabaseManager( options );

        if( DatabaseManager::instance == nullptr )
        {
            error = std::unique_ptr<QSqlError>(
                new QSqlError( "Error on instance creation", "Something went wrong", QSqlError::UnknownError )
            );
        }
        else
        {
            if( DatabaseManager::instance->database.isOpenError() || !DatabaseManager::instance->database.isOpen() )
            {
                error = std::unique_ptr<QSqlError>(
                    new QSqlError( DatabaseManager::instance->database.lastError() )
                );

                delete DatabaseManager::instance;
                DatabaseManager::instance = nullptr;
            }
        }
    }
    else
    {
        error = std::unique_ptr<QSqlError>(
            new QSqlError( "Error on instance creation", "Database file does not exists", QSqlError::UnknownError )
        );
    }

    return( error );
}


void DatabaseManager::clearInstance()
{
    if( DatabaseManager::instance != nullptr )
    {
        delete DatabaseManager::instance;
        DatabaseManager::instance = nullptr;
    }
}


DatabaseManager::DatabaseManager( const UniMap& options )
{
    QString driverName = options.value( "driver_name" ).toString();
    QString databaseName = options.value( "database_name" ).toString();

    this->database = QSqlDatabase::addDatabase( driverName, databaseName );

    this->database.setDatabaseName( databaseName );
    this->database.setHostName( options.value( "hostname" ).toString() );
    this->database.setPort( options.value( "port" ).toInt() );
    this->database.setUserName( options.value( "user_name" ).toString() );
    this->database.setPassword( options.value( "password" ).toString() );

    this->database.open();
}


DatabaseManager::~DatabaseManager()
{
    if( this->database.isOpen() )
    {
        this->database.commit();
        this->database.close();
    }

    QSqlDatabase::removeDatabase( this->database.connectionName() );
}
