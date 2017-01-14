/*************************************************************************************************
 *                                                                                                *
 *  file: version.h                                                                               *
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

#ifndef VERSION_H
#define VERSION_H

#include <QList>
#include <QObject>


namespace Application
{
      // Main information
    const QString appName = "spending-control";
    const QString appNameGui = "SpendingControl";
    const QString appLicense = "GNU GPL v2";
    const QString appWebsite = "-";
    const QString appDescription = QObject::tr( " -- description -- " ); // FIXME

      // Author
    const QString appAuthor = "Eugene Melnik <jeka7js@gmail.com>";
    const QString orgName   = "Eugene Melnik";
    const QString orgDomain = "eugene.melnik.com";

      // Versions
    const quint8  verMajor = 0;
    const quint8  verMinor = 1;
    const quint8  verFix = 0;

    const QString appVersionFull = QString( "%1.%2.%3" ).arg( verMajor ).arg( verMinor ).arg( verFix );
    const QString appBuildDate = QString( __DATE__ );

    const quint8  databaseVersion = 2;

    const quint8 settingsVersion = 1;

      // Locales
    struct Locale
    {
        QString title;
        QString selfTitle;
        QString name;
        QString translator;
    };

    const QList<Locale> supportedLocales =
    {
        { "English",    "English",     "en",     appAuthor }
    };
}


#endif // VERSION_H
