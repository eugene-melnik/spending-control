/*************************************************************************************************
 *                                                                                                *
 *  file: addtransactiondialog.cpp                                                                *
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

#include "model/transactionsmodel.h"
#include "addtransactiondialog.h"


AddTransactionDialog::AddTransactionDialog( QWidget* parent )
  : EditTransactionDialog( parent )
{
    this->setWindowTitle( tr( "Add transaction" ) );
}


void AddTransactionDialog::showEvent( QShowEvent* event )
{
    this->transactionId = 0;

    this->clearPageValues( TransactionsModel::Outgoing );
    this->clearPageValues( TransactionsModel::Incoming );
    this->clearPageValues( TransactionsModel::Internal );

    this->bTypeOutgoing->setChecked( true );
    this->swContent->setCurrentIndex( TransactionsModel::Outgoing );

    event->accept();
}
