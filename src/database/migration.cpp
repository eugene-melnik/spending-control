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

    this->setupMigrationsList();

    this->error.clear();
    this->database.transaction();

    while( currentVersion++ < version )
    {
        QStringList migrations = this->migrationsList.value( currentVersion );

        for( const QString& migration : migrations )
        {
            QString simplifiedQuery = migration.simplified();
            QSqlQuery query = this->database.exec( simplifiedQuery );

            if( query.lastError().isValid() )
            {
                this->database.rollback();

                this->clearMigrationsList();

                this->error = QString( "[migration %1, part \"%2\"] %3" ).arg(
                    QString::number( currentVersion ),
                    simplifiedQuery,
                    query.lastError().text()
                );

                return( false );
            }
        }
    };

    this->clearMigrationsList();

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


void DatabaseMigration::setupMigrationsList()
{
    /*
     * Create empty 'service' table and add initial 'migration_version' value.
     */
    this->migrationsList.insert(
        1,
        {
            "PRAGMA encoding = 'UTF-8';",
            "PRAGMA foreign_keys = ON;",

            "CREATE TABLE `service_options` ( \
                `name`              TEXT NOT NULL, \
                `value`             TEXT, \
                PRIMARY KEY(name) \
            );",

            "INSERT INTO service_options (name, value) VALUES ('migration_version', 0);"
        }
    );

    /*
     * Initial versions of tables 'accounts', 'categories' and 'transactions'.
     */
    this->migrationsList.insert(
        2,
        {
            "CREATE TABLE `accounts` ( \
                `id`                INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, \
                `name`              TEXT NOT NULL, \
                `description`       TEXT NOT NULL, \
                `type`              INTEGER NOT NULL DEFAULT '0', \
                `currency`          INTEGER NOT NULL DEFAULT '0', \
                `initial_balance`   INTEGER DEFAULT '0', \
                `minimal_balance`   INTEGER DEFAULT '0', \
                `created_at`        TEXT, \
                `closed_at`         TEXT \
            );",
            "CREATE INDEX accounts_name_idx ON accounts(name);",
            "CREATE INDEX accounts_type_idx ON accounts(type);",
            "CREATE INDEX accounts_created_at_idx ON accounts(created_at);",

            "CREATE TABLE `categories` ( \
                `id`                INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, \
                `name`              TEXT NOT NULL, \
                `description`       TEXT, \
                `parent_category_id` INTEGER DEFAULT '1', \
                `system`            INTEGER DEFAULT '0', \
                `deleted_at`        TEXT, \
                FOREIGN KEY (parent_category_id) REFERENCES categories(id) \
            );",
            "INSERT INTO categories (id, name, parent_category_id, system) VALUES (1, '<root>', null, 1);",

            "CREATE TABLE `transactions` ( \
                `id`                INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, \
                `type`              INTEGER NOT NULL DEFAULT '0', \
                `amount`            INTEGER NOT NULL DEFAULT '0', \
                `balance_after`     INTEGER NOT NULL DEFAULT '0', \
                `date`              TEXT NOT NULL, \
                `planned`           INTEGER DEFAULT '0', \
                `source_account_id` INTEGER NOT NULL, \
                `destination_account_id` INTEGER, \
                `category_id`       INTEGER NOT NULL, \
                `notes`             TEXT, \
                FOREIGN KEY (source_account_id) REFERENCES accounts(id), \
                FOREIGN KEY (destination_account_id) REFERENCES accounts(id), \
                FOREIGN KEY (category_id) REFERENCES categories(id) \
            );",
            "CREATE INDEX transactions_type_idx ON transactions(type);",
            "CREATE INDEX transactions_date_idx ON transactions(date);",
            "CREATE INDEX transactions_category_id_idx ON transactions(category_id);"
        }
    );

    /*
     * Initial version of table 'transaction_items'.
     */
    this->migrationsList.insert(
        3,
        {
            "CREATE TABLE `transaction_items` ( \
                `id`                INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, \
                `transaction_id`    INTEGER NOT NULL, \
                `name`              TEXT NOT NULL, \
                `amount`            INTEGER NOT NULL DEFAULT '0', \
                `category_id`       INTEGER NOT NULL, \
                FOREIGN KEY (transaction_id) REFERENCES transactions(id), \
                FOREIGN KEY (category_id) REFERENCES categories(id) \
            );",
            "CREATE INDEX transaction_items_transaction_id_idx ON transaction_items(transaction_id);",
            "CREATE INDEX transaction_items_category_id_idx ON transaction_items(category_id);"
        }
    );

    /*
     * Add type for categories.
     * Update root category.
     */
    this->migrationsList.insert(
        4,
        {
            "ALTER TABLE categories ADD COLUMN `type` INTEGER NOT NULL DEFAULT '0';",
            "UPDATE categories SET name = '<All>', type = 0 WHERE id = 1;"
        }
    );

    /*
     * Create default categories structure.
     */
    this->migrationsList.insert(
        5,
        {
            "INSERT INTO categories (id, parent_category_id, type, name) VALUES \
            (2, 1, 1, 'Car'), \
                (3, 2, 1, 'Fuel'), \
                (4, 2, 1, 'Repair'), \
                (5, 2, 1, 'Taxes'), \
            (6, 1, 0, 'Bank'), \
                (7, 6, 1, 'Comission'), \
                (8, 6, 1, 'Service charge'), \
                (9, 6, 1, 'Credit usage fee'), \
                (10, 6, 1, 'Mortgage payments'), \
                (11, 6, 2, 'Interest on deposit'), \
            (12, 1, 1, 'Charitable donations'), \
            (13, 1, 1, 'Household appliances'), \
            (14, 1, 2, 'Repayment of borrowed money'), \
            (15, 1, 1, 'Children'), \
                (16, 15, 1, 'Food'), \
                (17, 15, 1, 'Toys'), \
                (18, 15, 1, 'Education'), \
            (19, 1, 1, 'Pets'), \
                (20, 19, 1, 'Toys'), \
                (21, 19, 1, 'Nutrition'), \
                (22, 19, 1, 'Accessories'), \
                (23, 19, 1, 'Medicine'), \
                (24, 19, 1, 'Veterinary'), \
            (25, 1, 1, 'Leisure'), \
                (26, 25, 1, 'Restaurants and cafes'), \
                (27, 25, 1, 'Entertainment'), \
                    (28, 27, 1, 'Theater'), \
                    (29, 27, 1, 'Cinema'), \
                    (30, 27, 1, 'Cultural events'), \
                    (31, 27, 1, 'Sport events'), \
                (32, 25, 1, 'Music and video'), \
                (33, 25, 1, 'Games'), \
                    (34, 33, 1, 'Board games'), \
                    (35, 33, 1, 'Computer games'), \
            (36, 1, 2, 'Income from investments'), \
                (37, 36, 2, 'Capital gains'), \
                (38, 36, 2, 'Dividends'), \
                (39, 36, 2, 'Interest'), \
            (40, 1, 2, 'Other incomes'), \
                (41, 40, 2, 'Lottery'), \
                (42, 40, 2, 'Child benefits'), \
                (43, 40, 2, 'Unemployment compensation'), \
            (44, 1, 0, 'Miscellaneous'), \
            (45, 1, 2, 'Salary'), \
                (46, 45, 2, 'Bonuses'), \
                (47, 45, 2, 'Overtime'), \
            (48, 1, 1, 'Healthcare'), \
                (49, 48, 1, 'Hospital'), \
                (50, 48, 1, 'Stomatology'), \
                (51, 48, 1, 'Eye care'), \
                (52, 48, 1, 'Medicine'), \
                (53, 48, 1, 'Gym'), \
                (54, 48, 1, 'Sporting goods'), \
            (55, 1, 1, 'Cosmetics'), \
            (56, 1, 1, 'Personal hygiene'), \
            (57, 1, 1, 'Furniture'), \
            (58, 1, 1, 'Non-grocery goods'), \
            (59, 1, 1, 'Education'), \
                (60, 59, 1, 'Books'), \
                (61, 59, 1, 'Courses'), \
            (62, 1, 2, 'Selling of own things'), \
            (63, 1, 1, 'Public transport'), \
                (64, 63, 1, 'Bus'), \
                (65, 63, 1, 'Metro (Underground/Subway)'), \
                (66, 63, 1, 'Train'), \
                (67, 63, 1, 'Taxi'), \
            (68, 1, 1, 'Closing'), \
                (69, 68, 1, 'Outerwear'), \
                (70, 68, 1, 'Underwear'), \
                (71, 68, 1, 'Shoes'), \
            (72, 1, 0, 'Borrowed money'), \
            (73, 1, 1, 'Pension'), \
            (74, 1, 0, 'Gifts'), \
            (75, 1, 1, 'Food'), \
                (76, 75, 1, 'Vegetables'), \
                (77, 75, 1, 'Fruits'), \
                (78, 75, 1, 'Mushrooms'), \
                (79, 75, 1, 'Meat'), \
                (80, 75, 1, 'Fish'), \
                (81, 75, 1, 'Beverages'), \
                (82, 75, 1, 'Snacks'), \
                (83, 75, 1, 'Bakery products'), \
                (84, 75, 1, 'Cheese'), \
            (85, 1, 1, 'Traveling'), \
                (86, 85, 1, 'Lodging'), \
                (87, 85, 1, 'Travel'), \
                (88, 85, 1, 'Flight'), \
            (89, 1, 1, 'Expenses for work'), \
            (90, 1, 2, 'Stipend'), \
            (91, 1, 1, 'Insurance'), \
                (92, 91, 1, 'Health'), \
                (93, 91, 1, 'Life'), \
                (94, 91, 1, 'Auto'), \
                (95, 91, 1, 'Property'), \
            (96, 1, 1, 'Bills'), \
                (97, 95, 1, 'House rental'), \
                (98, 95, 1, 'Utilities'), \
                    (99, 97, 1, 'Water'), \
                    (100, 97, 1, 'Sewage'), \
                    (101, 97, 1, 'Gas'), \
                    (102, 97, 1, 'Electricity'), \
                (103, 95, 1, 'Garbage and recycling'), \
                (104, 95, 1, 'Cell phone'), \
                (105, 95, 1, 'Telephone'), \
                (106, 95, 1, 'Internet'), \
                (107, 95, 1, 'Newspapers and magazines'), \
            (108, 1, 1, 'Electronics');"
        }
    );
}


void DatabaseMigration::clearMigrationsList()
{
    this->migrationsList.clear();
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
