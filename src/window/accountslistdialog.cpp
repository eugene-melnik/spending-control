/*************************************************************************************************
 *                                                                                                *
 *  file: accountslistdialog.cpp                                                                  *
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

#include "accountslistdialog.h"


AccountsListDialog::AccountsListDialog( QWidget* parent )
  : QDialog( parent )
{
    this->setupUi( this );

    this->connect( this->bAdd, &QPushButton::clicked, this, &AccountsListDialog::addAccount );
    this->connect( this->bEdit, &QPushButton::clicked, this, &AccountsListDialog::editAccount );
    this->connect( this->bDelete, &QPushButton::clicked, this, &AccountsListDialog::deleteAccount );
}


void AccountsListDialog::setListModel( AccountsModel* model, int modelColumn )
{
    if( this->listModel != nullptr )
    {
        delete this->listModel;
    }

    this->listModel = model;
    this->lvAccounts->setModel( this->listModel );
    this->lvAccounts->setModelColumn( modelColumn );
}


AccountsModel* AccountsListDialog::getListModel() const
{
    return( this->listModel );
}
