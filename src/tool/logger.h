/*************************************************************************************************
 *                                                                                                *
 *  file: logger.h                                                                                *
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

#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QTime>

#include "types.h"


class Logger
{
    public:
        enum class Destination
        {
            UNKNOWN = 0, STDOUT, STDERR, FILE, //DATABASE
        };

        enum class Level
        {
            UNKNOWN = 0, DEBUG, INFO, WARN, ERROR, CRIT
        };

        /*!
         *
         */
        void log( const QString& message, Level level);

        void debug( const QString& message ) { this->log( message, Level::DEBUG ); }
        void info( const QString& message ) { this->log( message, Level::INFO ); }
        void warn( const QString& message ) { this->log( message, Level::WARN ); }
        void error( const QString& message ) { this->log( message, Level::ERROR ); }
        void crit( const QString& message ) { this->log( message, Level::CRIT ); }

        /*!
         *
         */
        void funcStart( const QString& funcName, UniMap arguments = UniMap() );

        /*!
         *
         */
        void funcDone( const QString& funcName );

        static bool setMinLevel( Level minLevel );
        static bool setMinLevelStr( const QString& levelStr );
        static bool setDestination( Destination destination );
        static bool setDestinationStr( const QString& destinationStr );
        static bool setTimeFormat( const QString& format );
        static bool setFileName( const QString& fileName );

        /*!
         *
         */
        static Logger* getInstance();

    protected:
        Logger() = default;
        ~Logger() = delete;

        QString getTimestamp() const;
        QString getLevelString( Level level ) const;

        void write( const QString& string );

    private:
        QMap<QString,QTime> functionTimers;

        static QMap<Level,QString> levelNames;
        static QMap<Destination,QString> destinationNames;

        static Level minLevel;
        static Destination destination;
        static QString timeFormat;
        static QString fileName;

        static Logger* instance;
};


#endif // LOGGER_H
