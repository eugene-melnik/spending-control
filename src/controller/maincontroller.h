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

#include "model/accountsmodel.h"
#include "model/categoriesmodel.h"
#include "model/transactionsmodel.h"
#include "model/transactionitemsmodel.h"
#include "window/accountslistdialog.h"
#include "window/addaccountdialog.h"
#include "window/mainwindow.h"
#include "window/addtransactiondialog.h"

#include <QApplication>
#include <QObject>


class MainController : public QObject
{
    Q_OBJECT

    public:
        MainController( QApplication& app );
        ~MainController();

    protected slots:
        void showAddTransaction();
        void addTransaction( const UniMap& fieldsData );

        void showManageAccounts();
        void showAddAccount();
        void addOrUpdateAccount( const UniMap& fieldsData );
        void showEditAccount();
        void showDeleteAccount();

        void showManageCategories();

        void exit();

    protected:
        QString getDefaultConfigDir() const;

        AccountsModel* getAccountsModel();
        TransactionsModel* getTransactionsModel();
        TransactionItemsModel* getTransactionItemsModel();
        CategoriesModel* getCategoriesModel();

    private:
        void handleCommandLine( QApplication& app );

        void setupDatabase();
        void createDialogs();
        void connectSignals();

        void preloadModels();
        void showAccountsStatus();
        void showTotalBalance();
        void showLastTransactions();

        void recalculateAccountBalance( const QDateTime& fromDate, int accountId );

        /**
         * Windows
         */
        MainWindow* mainWindow = nullptr;

        AddTransactionDialog* addTransactionDialog = nullptr;

        AccountsListDialog* accountsListDialog = nullptr;
        AddAccountDialog* addAccountDialog = nullptr;
        EditAccountDialog* editAccountDialog = nullptr;

        /**
         * Models
         */
        AccountsModel* accountsModel = nullptr;
        TransactionsModel* transactionsModel = nullptr;
        TransactionItemsModel* transactionItemsModel = nullptr;
        CategoriesModel* categoriesModel = nullptr;
};


#endif // MAIN_CONTROLLER_H
