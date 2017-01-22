/*************************************************************************************************
 *                                                                                                *
 *  file: accountslistdialog.h                                                                    *
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

#ifndef ACCOUNTSLISTDIALOG_H
#define ACCOUNTSLISTDIALOG_H

#include <QDialog>

#include "model/accountsmodel.h"
#include "ui_accountslistdialog.h"


class AccountsListDialog : public QDialog, protected Ui::AccountsListDialog
{
    Q_OBJECT

    public:
        explicit AccountsListDialog( QWidget* parent = nullptr );

        void setListModel( AccountsModel* model, int modelColumn = 0 );

        AccountsModel* getListModel() const;

        int getSelectedRow() const;
        QVariantList getSelectedRecord() const;

    signals:
        void addAccount();
        void editAccount();
        void deleteAccount();

    protected slots:
        void accountSelected( const QModelIndex& index );

    protected:
        AccountsModel* listModel = nullptr;
};

#endif // ACCOUNTSLISTDIALOG_H
