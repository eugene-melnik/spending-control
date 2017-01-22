/*************************************************************************************************
 *                                                                                                *
 *  file: editaccountdialog.cpp                                                                   *
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

#include "editaccountdialog.h"

#include <QMessageBox>


EditAccountDialog::EditAccountDialog( QWidget* parent )
  : QDialog( parent )
{
    this->setupUi( this );
    this->connect( this->bOk, &QPushButton::clicked, this, &EditAccountDialog::okClicked );
}


void EditAccountDialog::setTypes( QStringList types )
{
    this->cbType->clear();
    this->cbType->addItems( types );
}


void EditAccountDialog::setCurrencies( QStringList currencies )
{
    this->cbCurrency->clear();
    this->cbCurrency->addItems( currencies );
}


void EditAccountDialog::setValues( const UniMap& fieldsData )
{
    this->recordId = fieldsData.value( "id" ).toInt();

    this->eName->setText( fieldsData.value( "name" ).toString() );
    this->eDescription->setText( fieldsData.value( "description" ).toString() );
    this->cbType->setCurrentIndex( fieldsData.value( "type" ).toInt() );
    this->cbCurrency->setCurrentIndex( fieldsData.value( "currency" ).toInt() );
    this->sbInitialBalance->setValue( (double) fieldsData.value( "initial_balance" ).toInt() / 100.0 );
    this->sbMinimalBalance->setValue( (double) fieldsData.value( "minimal_balance" ).toInt() / 100.0 );
}


void EditAccountDialog::okClicked()
{
    if( this->eName->text().isEmpty() )
    {
        QMessageBox::information(
            this,
            tr( "Save account data" ),
            tr( "Field \"Name\" is required!" )
        );

        return;
    }

    UniMap fieldsData = {
        { "id",             this->recordId },
        { "name",           this->eName->text() },
        { "description",    this->eDescription->toPlainText() },
        { "type",           this->cbType->currentIndex() },
        { "currency",       this->cbCurrency->currentIndex() },
        { "initial_balance", this->sbInitialBalance->value() },
        { "minimal_balance", this->sbMinimalBalance->value() }
    };

    emit this->saveData( fieldsData );

    this->accept();
}
