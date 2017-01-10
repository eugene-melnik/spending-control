/*************************************************************************************************
 *                                                                                                *
 *  file: manager_tests.cpp                                                                       *
 *                                                                                                *
 *  SpendingControl Tests                                                                         *
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

#include "manager_tests.h"
#include "database/manager.h"
#include "types.h"

#include <QDebug>
#include <QtTest>


void DatabaseManagerTests::init()
{
    DatabaseManager::clearInstance();
}


void DatabaseManagerTests::cleanup()
{
    DatabaseManager::clearInstance();
}


void DatabaseManagerTests::createInstanceWrongDriver()
{
    QVERIFY( DatabaseManager::getInstance() == nullptr );

    UniMap options = {
        { "driver_name", "SQLITE" }
    };

    std::unique_ptr<QSqlError> error = DatabaseManager::setupInstance( options );

    QVERIFY( error != nullptr );
}

void DatabaseManagerTests::createInstanceNonExistantFile()
{
    QVERIFY( DatabaseManager::getInstance() == nullptr );

    UniMap options = {
        { "driver_name",        "QSQLITE" },
        { "database_name",      "some-non-existant-database-file.s3db" },
        { "create_non_exist",   false }
    };

    std::unique_ptr<QSqlError> error = DatabaseManager::setupInstance( options );

    QVERIFY( error != nullptr );
    QVERIFY( error->databaseText() == "Database file does not exists" );
    QVERIFY( error->driverText() == "Error on instance creation" );
}
