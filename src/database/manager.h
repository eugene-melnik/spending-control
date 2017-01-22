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

#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H


#include <memory>
#include <QSqlDatabase>
#include <QSqlError>

#include "types.h"


class DatabaseManager
{
    public:
        /*!
         * \brief Return manager instance if set up.
         */
        static DatabaseManager* getInstance() { return( DatabaseManager::instance ); }

        /*!
         * \brief Open connection (if not open yet), return null on success or error otherwise.
         *
         * Supported parameters:
         *  - 'driver_name' - If not specified, QSQLITE will be used. (See Driver and DriverNames).
         *  - 'database_name' - Database file name for QSQLITE, database name otherwise.
         *  - 'hostname'
         *  - 'port'
         *  - 'user_name'
         *  - 'password'
         *  - 'create_non_exist' - 'true' by default.
         */
        static std::unique_ptr<QSqlError> setupInstance( UniMap options );

        /*!
         * \brief Close connection if open and clearing the instance.
         */
        static void clearInstance();

        static QString getLastErrorText();

        enum Driver
        {
            QDB2,
            QIBASE,
            QMYSQL,
            QOCI,
            QODBC,
            QPSQL,
            QSQLITE2,
            QSQLITE,
            QSYMSQL,
            QTDS
        };

        QSqlDatabase& getDatabase();

    protected:
        DatabaseManager( const UniMap& options );
        ~DatabaseManager();

        static QString DriverNames[];
        static const Driver defaultDriver = DatabaseManager::Driver::QSQLITE;

    private:
        QSqlDatabase database;

        static DatabaseManager* instance;
};


#endif // DATABASE_MANAGER_H
