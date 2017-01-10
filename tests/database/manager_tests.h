/*************************************************************************************************
 *                                                                                                *
 *  file: manager_tests.h                                                                         *
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

#ifndef DATABASE_MANAGER_TESTS
#define DATABASE_MANAGER_TESTS

#include <QObject>

class DatabaseManagerTests : public QObject
{
    Q_OBJECT

    private slots:
        void init();
        void cleanup();

        void createInstanceWrongDriver();
        void createInstanceNonExistantFile();
};

#endif // DATABASE_MANAGER_TESTS
