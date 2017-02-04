/*************************************************************************************************
 *                                                                                                *
 *  file: addaccountdialog.cpp                                                                    *
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

#include "addaccountdialog.h"


AddAccountDialog::AddAccountDialog( QWidget* parent )
  : EditAccountDialog( parent )
{
    this->setWindowTitle( tr( "Add account" ) );
}


void AddAccountDialog::showEvent( QShowEvent* event )
{
    this->recordId = 0;

    this->eName->clear();
    this->eDescription->clear();
    this->cbType->setCurrentIndex( 0 );
    this->cbCurrency->setCurrentIndex( 0 );
    this->sbInitialBalance->setValue( 0.0 );
    this->sbMinimalBalance->setValue( 0.0 );

    event->accept();
}
