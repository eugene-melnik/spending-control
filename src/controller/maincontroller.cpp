/*************************************************************************************************
 *                                                                                                *
 *  file: maincontroller.cpp                                                                      *
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

#include "database/manager.h"
#include "database/migration.h"
#include "model/accountsmodel.h"
#include "tool/commandlineparser.h"
#include "tool/settings.h"
#include "defines.h"
#include "maincontroller.h"
#include "version.h"

#include <cstdio>
#include <QFileInfo>
#include <QProcessEnvironment>


MainController::MainController( QApplication& app )
{
    AppLogger->funcStart( "MainController::MainController" );

    this->handleCommandLine( app );
    this->setupDatabase();
    this->createDialogs();
    this->connectSignals();

    this->mainWindow->show();

    AppLogger->funcDone( "MainController::MainController" );
}


MainController::~MainController()
{
    delete this->mainWindow;
}


void MainController::showAddTransaction()
{
    //
}


void MainController::showManageAccounts()
{
    AppLogger->funcStart( "MainController::showManageAccounts" );

    if( this->accountsListDialog == nullptr )
    {
        AppLogger->debug( "AccountsListDialogCreation..." );
        this->accountsListDialog = new AccountsListDialog( this->mainWindow );

        this->connect( this->accountsListDialog, &AccountsListDialog::addAccount, this, &MainController::showAddAccount );
        this->connect( this->accountsListDialog, &AccountsListDialog::editAccount, this, &MainController::showEditAccount );
        this->connect( this->accountsListDialog, &AccountsListDialog::deleteAccount, this, &MainController::showDeleteAccount );
    }

    // FIXME: only once on creation?
    AccountsModel* accountsModel = new AccountsModel( DatabaseManager::getInstance()->getDatabase() );
    this->accountsListDialog->setListModel( accountsModel, 1 ); // 1 - "name"

    this->accountsListDialog->show();
    AppLogger->funcDone( "MainController::showManageAccounts" );
}


void MainController::showAddAccount()
{
    AppLogger->funcStart( "MainController::showAddAccount" );

    if( this->addAccountDialog == nullptr )
    {
        AppLogger->debug( "AddAccountDialog creation.." );

        this->addAccountDialog = new AddAccountDialog( this->accountsListDialog );
        this->addAccountDialog->setTypes( AccountsModel::getTypes() );
        this->addAccountDialog->setCurrencies( AccountsModel::getCurrencies() );

        this->connect( this->addAccountDialog, &AddAccountDialog::saveData, this, &MainController::addAccount );
    }

    this->addAccountDialog->show();
    AppLogger->funcDone( "MainController::showAddAccount" );
}


void MainController::addAccount( const UniMap& fieldsData )
{
    AppLogger->funcStart( "MainController::addAccount", fieldsData );

    AccountsModel* accountsModel = this->accountsListDialog->getListModel();
    accountsModel->addAccountRecord( fieldsData );

    AppLogger->funcDone( "MainController::addAccount" );
}


void MainController::showEditAccount()
{
    //
}


void MainController::updateAccount( const UniMap& fieldsData )
{
    //
}


void MainController::showDeleteAccount()
{
    //
}


void MainController::deleteAccount( int recordId )
{
    //
}


void MainController::showManageCategories()
{
    //
}


void MainController::exit()
{
    AppLogger->debug( "Exiting..." );

    this->mainWindow->hide();

    // save window settings etc.

    this->mainWindow->close();
}


QString MainController::getDefaultConfigDir() const
{
    QString configDir;

    #ifdef Q_OS_LINUX
        configDir = QProcessEnvironment::systemEnvironment().value( "XDG_CONFIG_HOME" );

        if( configDir.isEmpty() )
        {
            configDir = QProcessEnvironment::systemEnvironment().value( "HOME" ) + "/.config";
        }
    #elif defined(Q_OS_WIN32)
        configDir = QProcessEnvironment::systemEnvironment().value( "APPDATA" );
    #endif

    return( configDir + "/" + Application::appName );
}


void MainController::handleCommandLine( QApplication& app )
{
    CommandLineParser parser;
    parser.process( app );

    QString logDestination = parser.getLoggerDestination();
    QString logMinLevel = parser.getLoggerMinLevel();
    QString logFilename = parser.getLoggerFilename();

    if( !logDestination.isEmpty() )
    {
        bool ok = Logger::setDestinationStr( logDestination );

        if( !ok )
        {
            AppLogger->crit( "Not supported log destination: " + logDestination );
            fprintf( stderr, "Not supported log destination: %s\n", logDestination.toUtf8().data() );
            ::exit( 1 );
        }
    }

    if( !logMinLevel.isEmpty() )
    {
        bool ok = Logger::setMinLevelStr( logMinLevel );

        if( !ok )
        {
            AppLogger->crit( "Not supported log level: " + logMinLevel );
            fprintf( stderr, "Not supported log level: %s\n", logMinLevel.toUtf8().data() );
            ::exit( 1 );
        }
    }

    if( !logFilename.isEmpty() )
    {
        Logger::setFileName( logFilename );
    }

    QString configFilename = parser.getConfigLocation();

    if( configFilename.isEmpty() )
    {
        configFilename += this->getDefaultConfigDir() + "/configuration.conf";
    }

    AppLogger->info( "Config filename: " + configFilename );
    Settings::setupInstance( configFilename );
}


void MainController::setupDatabase()
{
    QString databaseFilename = Settings::getInstance()->getDatabaseFilename();

    if( databaseFilename.isEmpty() )
    {
        databaseFilename = QFileInfo( Settings::getInstance()->getConfigFilename() ).absolutePath() + "/database.s3db";
        Settings::getInstance()->setDatabaseFilename( databaseFilename );
    }

    AppLogger->info( "Database filename: " + databaseFilename );

    std::unique_ptr<QSqlError> error = DatabaseManager::setupInstance( {
        { "driver_name",    "QSQLITE" },
        { "database_name",  databaseFilename }
    } );

    if( error )
    {
        AppLogger->crit( "Database opening error: " + error->text() );
        fprintf( stderr, "Database opening error: %s\n", error->text().toUtf8().data() );
        ::exit( 1 );
    }

    DatabaseMigration migrator( DatabaseManager::getInstance()->getDatabase() );

    AppLogger->info( QString( "Migrate database from version %1 to version %2" )
        .arg( migrator.getCurrentVersion() )
        .arg( Application::databaseVersion )
    );

    if( migrator.migrateToVersion( Application::databaseVersion ) )
    {
        AppLogger->info( "Done" );
    }
    else
    {
        AppLogger->crit( "Database migration error: " + migrator.getLastError() );
        fprintf( stderr, "Database migration error: %s\n", migrator.getLastError().toUtf8().data() );
        ::exit( 1 );
    }
}


void MainController::createDialogs()
{
    AppLogger->debug( "MainController::createDialogs()" );

    this->mainWindow = new MainWindow();
}


void MainController::connectSignals()
{
    AppLogger->debug( "MainController::connectSignals()" );

    this->connect( this->mainWindow, &MainWindow::addTransaction, this, &MainController::showAddTransaction );
    this->connect( this->mainWindow, &MainWindow::manageAccounts, this, &MainController::showManageAccounts );
    this->connect( this->mainWindow, &MainWindow::manageCategories, this, &MainController::showManageCategories );

    this->connect( this->mainWindow, &MainWindow::aboutToClose, this, &MainController::exit );
}
