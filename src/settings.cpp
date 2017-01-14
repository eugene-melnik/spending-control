/*************************************************************************************************
 *                                                                                                *
 *  file: settings.cpp                                                                            *
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

#include "settings.h"
#include "version.h"

Settings* Settings::instance = nullptr;


void Settings::setupInstance( const QString& configFilename )
{
    Settings::clearInstance();
    Settings::instance = new Settings( configFilename );
}


void Settings::clearInstance()
{
    if( Settings::instance != nullptr )
    {
        delete Settings::instance;
        Settings::instance = nullptr;
    }
}


Settings::Settings( const QString& configFilename )
    : QSettings( configFilename, QSettings::Format::IniFormat )
{
    int version = this->getSettingsVersion();

    if( version < Application::settingsVersion )
    {
        // migration to new version
    }

    this->setSettingsVersion( Application::settingsVersion );
    this->sync();
}


int Settings::getSettingsVersion() const
{
    return( this->getValue( "application", "settings_version", 0 ).toInt() );
}


void Settings::setSettingsVersion( int version )
{
    this->setValue( "application", "settings_version", version );
}


QVariant Settings::getValue( const QString& section, const QString& key, QVariant defaultValue ) const
{
    return( this->value( section + "/" + key, defaultValue ) );
}


void Settings::setValue( const QString& section, const QString& key, QVariant value )
{
    QSettings::setValue( section + "/" + key, value );
}
