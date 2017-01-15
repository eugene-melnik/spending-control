/*************************************************************************************************
 *                                                                                                *
 *  file: settings.h                                                                              *
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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QString>


class Settings : protected QSettings
{
    public:
        /*!
         *
         */
        static Settings* getInstance() { return( Settings::instance ); }

        /*!
         *
         */
        static void setupInstance( const QString& configFilename );

        /*!
         *
         */
        static void clearInstance();

        /*!
         *
         */
        QString getConfigFilename() const;

        /* Get */

        QString getDatabaseFilename() const;

        /* Set */

        void setDatabaseFilename( const QString& filename );

    protected:
        Settings( const QString& configFilename );
        ~Settings() = default;

        int getSettingsVersion() const;
        void setSettingsVersion( int version );

        QVariant getValue( const QString& section, const QString& key, QVariant defaultValue = QVariant() ) const;
        void setValue( const QString& section, const QString& key, QVariant value );

    private:
        static Settings* instance;
};


#endif // SETTINGS_H
