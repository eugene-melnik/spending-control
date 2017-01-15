/*************************************************************************************************
 *                                                                                                *
 *  file: commandlineparser.cpp                                                                   *
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

#include "commandlineparser.h"
#include "version.h"

#include <QApplication>
#include <QStringList>


CommandLineParser::CommandLineParser() : QCommandLineParser()
{
    this->setApplicationDescription( Application::appDescription );

    // "-h" or "--help"
    this->addHelpOption();

    // "-v" or "--version"
    this->addVersionOption();

    // "-c" or "--config"
    this->addConfigOption();

    // "--log-destination"
    // "--log-min-level"
    // "--log-filename"
    this->addLoggerOptions();
}


QString CommandLineParser::getConfigLocation() const
{
    return( this->value( "config" ) );
}


QString CommandLineParser::getLoggerDestination() const
{
    return( this->value( "log-destination" ) );
}


QString CommandLineParser::getLoggerMinLevel() const
{
    return( this->value( "log-min-level" ) );
}


QString CommandLineParser::getLoggerFilename() const
{
    return( this->value( "log-filename" ) );
}


void CommandLineParser::addConfigOption()
{
    QCommandLineOption configLocationOption(
        QStringList() << "c" << "config",
        "Specify the location of the configuration file.",
        "/path/to/config"
    );

    this->addOption( configLocationOption );
}


void CommandLineParser::addLoggerOptions()
{
    QString defaultDestinationValue = "stdout";
    QString defaultLevelValue = "debug";

    QCommandLineOption logDestinationOption(
        "log-destination",
        "Specify the type of logging.",
        "stdout|stderr|file",
        defaultDestinationValue
    );

    this->addOption( logDestinationOption );

    QCommandLineOption logMinLevelOption(
        "log-min-level",
        "Minimum level for logging.",
        "debug|info|warning|error|crit",
        defaultLevelValue
    );

    this->addOption( logMinLevelOption );

    QCommandLineOption logFilenameOption(
        "log-filename",
        "Specify the location of logfile .",
        "/path/to/file"
    );

    this->addOption( logFilenameOption );
}
