/*************************************************************************************************
 *                                                                                                *
 *  file: logger.cpp                                                                              *
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

#include "logger.h"
#include "version.h"

#include <cstdio>
#include <QDateTime>


QMap<Logger::Level,QString> Logger::levelNames = {
    { Logger::Level::DEBUG, "DEBUG" },
    { Logger::Level::INFO,  "INFO" },
    { Logger::Level::WARN,  "WARNING" },
    { Logger::Level::ERROR, "ERROR" },
    { Logger::Level::CRIT,  "CRITICAL" },
};

QMap<Logger::Destination,QString> Logger::destinationNames = {
    { Logger::Destination::STDOUT, "STDOUT" },
    { Logger::Destination::STDERR, "STDERR" },
    { Logger::Destination::FILE,   "FILE" },
};

#ifdef QT_DEBUG
    Logger::Level Logger::minLevel = Logger::Level::DEBUG;
#else
    Logger::Level Logger::minLevel = Logger::Level::ERROR;
#endif

#ifdef Q_OS_LINUX
    QString Logger::fileName = "/var/log/" + Application::appName + ".log";
#elif defined(Q_OS_WIN)
    QString Logger::fileName = "C:\\" + Application::appName + ".log"; // FIXME
#else
    QString Logger::fileName = "";
#endif

Logger::Destination Logger::destination = Logger::Destination::STDOUT;
QString Logger::timeFormat = "yyyy-MM-dd hh:mm:ss.zzz";

Logger* Logger::instance = nullptr;


void Logger::log( const QString& message, const UniMap& values, Logger::Level level )
{
    if( level >= Logger::minLevel )
    {
        if( message.isEmpty() && values.isEmpty() )
        {
            this->write( "\n" );
        }
        else
        {
            this->write( QString( "[%1] -- %2 -- %3\n" ).arg(
                this->getTimestamp(),
                this->getLevelString( level ),
                message + ( values.isEmpty() ? "" : " -- " + this->formatUnimapValues( values ) )
            ) );
        }
    }
}


void Logger::log( const QString& message, const QVariantList& list, Logger::Level level )
{
    if( level >= Logger::minLevel )
    {
        if( message.isEmpty() && list.isEmpty() )
        {
            this->write( "\n" );
        }
        else
        {
            this->write( QString( "[%1] -- %2 -- %3\n" ).arg(
                this->getTimestamp(),
                this->getLevelString( level ),
                message + ( list.isEmpty() ? "" : " -- " + this->formatListValues( list ) )
            ) );
        }
    }
}


void Logger::funcStart( const QString& funcName, UniMap arguments )
{
    QString message = QString( "%1(%2)" ).arg( funcName, this->formatUnimapValues( arguments ) );

    QTime startTime;
    startTime.start();

    this->functionTimers.insert( funcName, startTime );

    this->log( "Starting: " + message, UniMap(), Level::DEBUG );
}


void Logger::funcDone( const QString& funcName, UniMap arguments )
{
    int timeElapsed = this->functionTimers.value( funcName ).elapsed();
    QString timeUnit = "ms";

    if( timeElapsed / 1000 > 2 )
    {
        timeElapsed /= 1000;
        timeUnit = "s";
    }

    this->log( QString( "Done: %1 in %2 %3" ).arg(
        funcName + ( arguments.isEmpty() ? "" : "(" + this->formatUnimapValues( arguments ) + ")"),
        QString::number( timeElapsed ),
        timeUnit
    ), UniMap(), Level::DEBUG );

    this->functionTimers.remove( funcName );
}


bool Logger::setMinLevel( Logger::Level minLevel )
{
    Logger::minLevel = minLevel;
    return true;
}


bool Logger::setMinLevelStr( const QString& levelStr )
{
    QString levelStrUp = levelStr.toUpper();

    if( Logger::levelNames.key( levelStrUp, Level::UNKNOWN ) != Level::UNKNOWN )
    {
        Logger::minLevel = Logger::levelNames.key( levelStrUp );
        return true;
    }
    else
    {
        return false;
    }
}


bool Logger::setDestination( Logger::Destination destination )
{
    Logger::destination = destination;
    return true;
}


bool Logger::setDestinationStr( const QString& destinationStr )
{
    QString destinationStrUp = destinationStr.toUpper();

    if( Logger::destinationNames.key( destinationStrUp, Destination::UNKNOWN ) != Destination::UNKNOWN )
    {
        Logger::destination = Logger::destinationNames.key( destinationStrUp );
        return true;
    }
    else
    {
        return false;
    }
}


bool Logger::setTimeFormat( const QString& format )
{
    Logger::timeFormat = format;
    return true;
}


bool Logger::setFileName( const QString& fileName )
{
    Logger::fileName = fileName;
    return true;
}


Logger* Logger::getInstance()
{
    if( Logger::instance == nullptr )
    {
        Logger::instance = new Logger();
    }

    return Logger::instance;
}


QString Logger::getTimestamp() const
{
    return( QDateTime::currentDateTime().toString( Logger::timeFormat ) );
}


QString Logger::formatUnimapValues( const UniMap& values, bool withNewLines )
{
    // TODO: add support of subarrays etc.

    QString result;

    for( const QString& key : values.keys() )
    {
        result += QString( "%1='%2'%3" ).arg(
            key,
            values.value( key ).toString(),
            withNewLines ? "\n" : " "
        );
    }

    return( result );
}


QString Logger::formatListValues( const QVariantList& list, bool withNewLines )
{
    QString result;

    for( const QVariant& value : list )
    {
        result += QString( "'%1'%2" ).arg(
            value.toString(),
            withNewLines ? "\n" : " "
        );
    }

    return( result );
}


QString Logger::getLevelString( Logger::Level level ) const
{
    QString levelName = Logger::levelNames.value( level );
    return( levelName.isEmpty() ? "<unknown>" : levelName );
}


void Logger::write( const QString& string )
{
    switch( Logger::destination )
    {
        case Destination::STDOUT :
        {
            fputs( string.toUtf8().data(), stdout );
            break;
        }

        case Destination::STDERR :
        {
            fputs( string.toUtf8().data(), stderr );
            break;
        }

        case Destination::FILE :
        {
            // not confuse to Logger::Destination::FILE
            ::FILE* logfile = fopen( Logger::fileName.toUtf8().data(), "a" );

            if( logfile != NULL )
            {
                fputs( string.toUtf8().data(), logfile );
                fclose( logfile );
            }
            else
            {
                QString message = "Can't open log file '" + Logger::fileName + "' for append.";
                fputs( message.toUtf8().data() , stderr );
            }

            break;
        }

        default :
        {
            QString message = "Unsupported destination method: " + QString::number( (int) Logger::destination );
            fputs( message.toUtf8().data() , stderr );
            break;
        }
    }
}
