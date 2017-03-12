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
#include "tool/commandlineparser.h"
#include "tool/settings.h"
#include "defines.h"
#include "maincontroller.h"
#include "version.h"

#include <cstdio>
#include <QFileInfo>
#include <QMessageBox>
#include <QProcessEnvironment>
#include <QSqlQueryModel>
#include <QSqlRecord>


MainController::MainController( QApplication& app )
{
    AppLogger->funcStart( "MainController::MainController" );

    this->handleCommandLine( app );

    this->setupDatabase();
    this->createDialogs();
    this->connectSignals();

    this->preloadModels();

    this->showAccountsStatus();
    this->showTotalBalance();
    this->showLastTransactions();

    this->mainWindow->show();

    AppLogger->funcDone( "MainController::MainController" );
}


MainController::~MainController()
{
    delete this->mainWindow;

    if( this->accountsModel != nullptr )
    {
        delete this->accountsModel;
    }

    if( this->transactionsModel != nullptr )
    {
        delete this->transactionsModel;
    }

    if( this->transactionItemsModel != nullptr )
    {
        delete this->transactionItemsModel;
    }
}


void MainController::showAddTransaction()
{
    AppLogger->funcStart( "MainController::showAddTransaction" );

    if( this->addTransactionDialog == nullptr )
    {
        AppLogger->debug( "AddTransactionDialog creation..." );

        this->addTransactionDialog = new AddTransactionDialog( this->mainWindow );

        this->connect( this->addTransactionDialog, &AddTransactionDialog::saveData, this, &MainController::addTransaction );
    }

    this->addTransactionDialog->setAccountsList( this->getAccountsModel()->getList() );
    this->addTransactionDialog->setCategoriesList( this->getCategoriesModel()->getList() );

    this->addTransactionDialog->show();

    AppLogger->funcDone( "MainController::showAddTransaction" );
}


void MainController::addTransaction( const UniMap& fieldsData )
{
    AppLogger->funcStart( "MainController::addTransaction", fieldsData );

    unsigned int transactionId = 0;
    bool ok = this->getTransactionsModel()->addRecord( fieldsData, &transactionId );

    if( !ok )
    {
        QMessageBox::warning(
            this->mainWindow,
            tr( "Save transaction data" ),
            tr( "Internal error occurred, please see log file for details." )
        );

        return;
    }

    int sourceAccountId = fieldsData.value( "source_account_id" ).toInt();
    int destinationAccountId = fieldsData.value( "destination_account_id" ).toInt();
    bool internal = fieldsData.value( "type" ).toInt() == TransactionsModel::Internal;

    if( internal )
    {
        UniMap fieldsDataCopy = fieldsData;

        // Source account transaction
        fieldsDataCopy.insert( "type", TransactionsModel::Outgoing );
        fieldsDataCopy.remove( "destination_account_id" );
        ok = this->getTransactionsModel()->addRecord( fieldsDataCopy );

        // Destination account transaction
        fieldsDataCopy.insert( "type", TransactionsModel::Incoming );
        fieldsDataCopy.insert( "destination_account_id", destinationAccountId );
        fieldsDataCopy.remove( "source_account_id" );
        ok &= this->getTransactionsModel()->addRecord( fieldsDataCopy );

        if( !ok )
        {
            QMessageBox::warning(
                this->mainWindow,
                tr( "Save transaction data" ),
                tr( "Internal error occurred, please see log file for details." )
            );
        }
    }
    else if( fieldsData.contains( "transaction_subitems" ) )
    {
        QList<QVariantList> subitems = fieldsData.value( "transaction_subitems" ).value<QList<QVariantList>>();
        TransactionItemsModel* transactionItemsModel = this->getTransactionItemsModel();

        for( const QVariantList& rowData : subitems )
        {
            QString name = rowData.at( 0 ).toString();

            bool added = transactionItemsModel->addRecord({
                { "transaction_id", transactionId },
                { "name",           name },
                { "category_id",    rowData.at( 1 ) },
                { "amount",         rowData.at( 2 ) }
            });

            if( !added )
            {
                QMessageBox::warning(
                    this->mainWindow,
                    tr( "Save transaction data" ),
                    tr( "Internal error - can't save item \"%1\", please see log file for details." ).arg( name )
                );
            }
        }
    }

    QDateTime transactionDate = QDateTime::fromString( fieldsData.value( "date" ).toString(), Qt::ISODate );

    if( sourceAccountId )
    {
        this->recalculateAccountBalance( transactionDate, sourceAccountId );
    }

    if( destinationAccountId )
    {
        this->recalculateAccountBalance( transactionDate, destinationAccountId );
    }

    this->showAccountsStatus();
    this->showTotalBalance();
    this->showLastTransactions();

    AppLogger->funcDone( "MainController::addTransaction" );
}


void MainController::showManageAccounts()
{
    AppLogger->funcStart( "MainController::showManageAccounts" );

    if( this->accountsListDialog == nullptr )
    {
        AppLogger->debug( "AccountsListDialog creation..." );

        this->accountsListDialog = new AccountsListDialog( this->mainWindow );
        this->accountsListDialog->setListModel( this->getAccountsModel(), 1 ); // 1 -- "name" column

        this->connect( this->accountsListDialog, &AccountsListDialog::addAccount, this, &MainController::showAddAccount );
        this->connect( this->accountsListDialog, &AccountsListDialog::editAccount, this, &MainController::showEditAccount );
        this->connect( this->accountsListDialog, &AccountsListDialog::deleteAccount, this, &MainController::showDeleteAccount );
    }

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

        this->connect( this->addAccountDialog, &AddAccountDialog::saveData, this, &MainController::addOrUpdateAccount );
    }

    this->addAccountDialog->show();

    AppLogger->funcDone( "MainController::showAddAccount" );
}


void MainController::addOrUpdateAccount( const UniMap& fieldsData )
{
    AppLogger->funcStart( "MainController::addOrUpdateAccount", fieldsData );

    AccountsModel* accountsModel = this->getAccountsModel();
    bool ok;

    if( fieldsData.value( "id" ).toInt() == 0 )
    {
        ok = accountsModel->addRecord( fieldsData );
    }
    else
    {
        ok = accountsModel->updateRecord( fieldsData );
    }

    if( !ok )
    {
        QMessageBox::warning(
            this->mainWindow,
            tr( "Save account data" ),
            tr( "Internal error occurred, please see log file for details." )
        );

        return;
    }

    this->showAccountsStatus();
    this->showTotalBalance();
    this->showLastTransactions();

    AppLogger->funcDone( "MainController::addOrUpdateAccount" );
}


void MainController::showEditAccount()
{
    AppLogger->funcStart( "MainController::showEditAccount" );

    if( this->editAccountDialog == nullptr )
    {
        AppLogger->debug( "EditAccountDialog creation..." );

        this->editAccountDialog = new EditAccountDialog( this->accountsListDialog );
        this->editAccountDialog->setTypes( AccountsModel::getTypes() );
        this->editAccountDialog->setCurrencies( AccountsModel::getCurrencies() );

        this->connect( this->editAccountDialog, &EditAccountDialog::saveData, this, &MainController::addOrUpdateAccount );
    }

    int selectedRow = this->accountsListDialog->getSelectedRow();
    this->editAccountDialog->setValues( this->getAccountsModel()->getRecordMapped( selectedRow ) );
    this->editAccountDialog->show();

    AppLogger->funcDone( "MainController::showEditAccount" );
}


void MainController::showDeleteAccount()
{
    AppLogger->funcStart( "MainController::showDeleteAccount" );

    QVariantList recordData = this->accountsListDialog->getSelectedRecord();
    QString accountName = recordData.at( 1 ).toString();

    int button = QMessageBox::question(
        this->mainWindow,
        tr( "Delete account" ),
        tr( "Are you sure you want to delete selected account (\"%1\")?" ).arg( accountName )
    );

    if( button == QMessageBox::Yes )
    {
        int accountId = recordData.at( 0 ).toInt();
        bool deleted = this->getAccountsModel()->deleteOrCloseAccount( accountId );

        if( deleted )
        {
            QMessageBox::information(
                this->mainWindow,
                tr( "Delete account" ),
                tr( "Account \"%1\" successfully deleted!" ).arg( accountName )
            );
        }
        else
        {
            QMessageBox::warning(
                this->mainWindow,
                tr( "Delete account" ),
                tr( "Internal error occurred, please see log file for details." )
            );
        }
    }

    AppLogger->funcDone( "MainController::showDeleteAccount" );
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


AccountsModel* MainController::getAccountsModel()
{
    if( this->accountsModel == nullptr )
    {
        this->accountsModel = new AccountsModel( DatabaseManager::getInstance()->getDatabase() );
    }

    return( this->accountsModel );
}


TransactionsModel* MainController::getTransactionsModel()
{
    if( this->transactionsModel == nullptr )
    {
        this->transactionsModel = new TransactionsModel( DatabaseManager::getInstance()->getDatabase() );
    }

    return( this->transactionsModel );
}


TransactionItemsModel* MainController::getTransactionItemsModel()
{
    if( this->transactionItemsModel == nullptr )
    {
        this->transactionItemsModel = new TransactionItemsModel( DatabaseManager::getInstance()->getDatabase() );
    }

    return( this->transactionItemsModel );
}


CategoriesModel* MainController::getCategoriesModel()
{
    if( this->categoriesModel == nullptr )
    {
        this->categoriesModel = new CategoriesModel( DatabaseManager::getInstance()->getDatabase() );
    }

    return( this->categoriesModel );
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


void MainController::preloadModels()
{
    AppLogger->debug( "MainController::preloadModels()" );

    this->getAccountsModel();
    this->getTransactionsModel();
}


void MainController::showAccountsStatus()
{
    QSqlQueryModel* accountsStatusModel = new QSqlQueryModel( this->mainWindow );

    accountsStatusModel->setQuery(
        "SELECT \
            a.name AS \"Account name\", \
            CASE WHEN t.balance_after <> 0 THEN \
                printf('%.2f', t.balance_after / 100.0) \
            ELSE \
                printf('%.2f', a.initial_balance) / 100.0 \
            END AS \"Current balance\", \
            a.currency AS \"Currency\" \
        FROM \
            accounts a \
        LEFT JOIN (SELECT * FROM transactions ORDER BY date ASC) t \
            ON t.source_account_id = a.id OR t.destination_account_id = a.id \
        GROUP BY a.id \
        ORDER BY a.id ASC",
        DatabaseManager::getInstance()->getDatabase()
    );

    this->mainWindow->setAccountsStatusModel( accountsStatusModel );
}


void MainController::showTotalBalance()
{
    QSqlQuery query(
        "SELECT \
            printf('%.2f', SUM(current_balance / 100.0)) AS \"Total balance\" \
        FROM \
            (SELECT \
                CASE WHEN t.balance_after <> 0 THEN \
                    t.balance_after \
                ELSE \
                    a.initial_balance \
                END AS \"current_balance\" \
            FROM \
                accounts a \
            LEFT JOIN (SELECT * FROM transactions ORDER BY date ASC) t \
                ON t.source_account_id = a.id OR t.destination_account_id = a.id \
            GROUP BY a.id \
            ORDER BY a.id ASC \
        )",
        DatabaseManager::getInstance()->getDatabase()
    );

    query.first();

    this->mainWindow->setTotalBalance( query.value( 0 ).toString() );
}


void MainController::showLastTransactions()
{
    QSqlQueryModel* lastTransactionsModel = new QSqlQueryModel( this->mainWindow );

    lastTransactionsModel->setQuery(
        "SELECT \
            printf('%.2f', t.amount / 100.0) || ' UAH' AS \"Amount\", \
            strftime('%d.%m.%Y %H:%m', t.date) AS \"Date\", \
            CASE WHEN COUNT(ti.id) > 0 THEN \
                GROUP_CONCAT(ti.name, ', ') \
            ELSE \
                t.notes \
            END AS \"Description\" \
        FROM \
            transactions t \
        LEFT JOIN \
            transaction_items ti ON t.id = ti.transaction_id \
        WHERE \
            (SELECT COUNT(id) FROM transactions WHERE date = t.date) <> 3 \
        GROUP BY \
            t.id \
        ORDER BY \
            t.date DESC",
        DatabaseManager::getInstance()->getDatabase()
    );

    this->mainWindow->setLastTransactionsModel( lastTransactionsModel );
}


void MainController::recalculateAccountBalance( const QDateTime& fromDate, int accountId )
{
    AppLogger->funcStart( "MainController::recalculateBalance", {
        { "fromDate", fromDate.toString( Qt::ISODate ) },
        { "accountId", accountId }
    } );

    TransactionsModel* transactionsModel = this->getTransactionsModel();
    QList<UniMap> rowsToChange;

    for( int row = transactionsModel->rowCount() - 1; row >= 0; --row )
    {
        UniMap rowData = transactionsModel->getRecordMapped( row );

        QDateTime rowDate = QDateTime::fromString( rowData.value( "date" ).toString(), Qt::ISODate );
        bool prepared = rowData.value( "prepared" ).toBool();
        bool internal = rowData.value( "type" ).toInt() == TransactionsModel::Internal;
        int rowSourceAccount = rowData.value( "source_account_id" ).toInt();
        int rowDestinationAccount = rowData.value( "destination_account_id" ).toInt();

        if( !internal && !prepared && (rowSourceAccount == accountId || rowDestinationAccount == accountId) )
        {
            rowsToChange.prepend( rowData );

            if( rowDate < fromDate )
            {
                break;
            }
        }
    }

    UniMap firstRowData;
    QDateTime firstRowDate;
    int balance;

    if( rowsToChange.count() )
    {
        firstRowData = rowsToChange.first();
        firstRowDate = QDateTime::fromString( firstRowData.value( "date" ).toString(), Qt::ISODate );
    }
    else
    {
        firstRowDate = QDateTime::currentDateTime();
    }

    if( firstRowDate < fromDate )
    {
        balance = firstRowData.value( "balance_after" ).toInt();
        rowsToChange.removeAt( 0 );
    }
    else // First transaction
    {
        UniMap accountData = AccountsModel::getById( accountId, DatabaseManager::getInstance()->getDatabase() );
        balance = accountData.value( "initial_balance" ).toInt();
    }

    for( UniMap rowData : rowsToChange )
    {
        if( rowData.value( "source_account_id" ).toInt() == accountId )
        {
            balance -= rowData.value( "amount" ).toInt();
        }
        else if( rowData.value( "destination_account_id" ).toInt() == accountId )
        {
            balance += rowData.value( "amount" ).toInt();
        }
        else
        {
            continue;
        }

        rowData.insert( "balance_after", balance );
        transactionsModel->updateRecord( rowData );
    }

    AppLogger->funcDone( "MainController::recalculateBalance" );
}
