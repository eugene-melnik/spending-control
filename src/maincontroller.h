/*************************************************************************************************
 *                                                                                                *
 *  file: maincontroller.h                                                                        *
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

#ifndef MAIN_CONTROLLER_H
#define MAIN_CONTROLLER_H

#include "mainwindow.h"

#include <QApplication>
#include <QObject>


class MainController : public QObject
{
    Q_OBJECT

    public:
        MainController( QApplication& app );
        ~MainController();

    public slots:
        void exit();

    protected:
        QString getDefaultConfigDir() const;

    private:
        void handleCommandLine( QApplication& app );
        void setupDatabase();
        void createDialogs();
        void connectSignals();

        MainWindow* mainWindow;
};


#endif // MAIN_CONTROLLER_H
