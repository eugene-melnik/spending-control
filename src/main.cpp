/*************************************************************************************************
 *                                                                                                *
 *  file: main.cpp                                                                                *
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

#include "controller/maincontroller.h"
#include "defines.h"
#include "version.h"

#include <QApplication>
#include <memory>


int main( int argc, char** argv )
{
    AppLogger->funcStart( "main", {{ "argc", argc }} );

    QApplication::setOrganizationName( Application::orgName );
    QApplication::setOrganizationDomain( Application::orgDomain );

    QApplication::setApplicationName( Application::appName );
    QApplication::setApplicationDisplayName( Application::appNameGui );
    QApplication::setApplicationVersion( Application::appVersionFull );

    QApplication spendingControl( argc, argv );
    std::unique_ptr<MainController> mainController( new MainController( spendingControl ) );

    int result = spendingControl.exec();

    AppLogger->funcDone( "main" );
    AppLogger->debug( "" );

    return( result );
}
