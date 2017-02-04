/*************************************************************************************************
 *                                                                                                *
 *  file: edittransactiondialog.cpp                                                               *
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
#include "edittransactiondialog.h"

#include <QDateTime>


EditTransactionDialog::EditTransactionDialog( QWidget* parent )
  : QDialog( parent )
{
    this->setupUi( this );
    this->setupSubitemsWidget();

    this->connect( this->typesButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(changeType()) );
    this->connect( this->bSetCurrentDate, &QPushButton::clicked, this, &EditTransactionDialog::setCurrentDate );
    this->connect( this->bOk, &QPushButton::clicked, this, &EditTransactionDialog::okClicked );
}


EditTransactionDialog::~EditTransactionDialog()
{
    delete this->subitemAmountDelegate;
}


void EditTransactionDialog::setAccountsList( const UniMap& accounts )
{
    this->cbAccountOutgoing->clear();
    this->cbDestinationAccountIncoming->clear();
    this->cbDestinationAccountInternal->clear();
    this->cbSourceAccountInternal->clear();

    for( const QString& id : accounts.keys() )
    {
        QString title = accounts.value( id ).toString();

        this->cbAccountOutgoing->addItem( title, id );
        this->cbDestinationAccountIncoming->addItem( title, id );
        this->cbDestinationAccountInternal->addItem( title, id );
        this->cbSourceAccountInternal->addItem( title, id );
    }
}

void EditTransactionDialog::setCategoriesList( const UniMap& categories )
{
    this->cbCategoryOutgoing->clear();
    this->cbCategoryIncoming->clear();

    for( const QString& id : categories.keys() )
    {
        QString title = categories.value( id ).toString();
        this->cbCategoryOutgoing->addItem( title, id );
        this->cbCategoryIncoming->addItem( title, id );
    }

    this->subitemCategoryDelegare->setValuesList( categories );
}


void EditTransactionDialog::setValues( const UniMap& fieldsData )
{
    QString date = fieldsData.value( "date" ).toString();
    this->wCalendar->setSelectedDate( QDate::fromString( date , Qt::ISODate ) );
    this->eDatetime->setDateTime( QDateTime::fromString( date , Qt::ISODate ) );

    int type = fieldsData.value( "type" ).toInt();

    switch( type )
    {
        case TransactionsModel::Type::OUTGOING :
        {
            this->bTypeOutgoing->setChecked( true );

            //

            break;
        }

        case TransactionsModel::Type::INCOMING :
        {
            this->bTypeIncoming->setChecked( true );

            //

            break;
        }

        case TransactionsModel::Type::INTERNAL :
        {
            this->bTypeInternal->setChecked( true );

            //

            break;
        }
    }

    this->swContent->setCurrentIndex( type );
}


void EditTransactionDialog::setCurrentDate()
{
    this->wCalendar->setSelectedDate( QDate::currentDate() );
    this->eDatetime->setDateTime( QDateTime::currentDateTime() );
}


void EditTransactionDialog::changeType()
{
    // TODO: reset pages content

    this->swContent->setCurrentIndex( this->getCurrentType() );
}


void EditTransactionDialog::createSubitem()
{
    int newRowId = this->twSubitems->rowCount();

    QTableWidgetItem* lastItem = this->twSubitems->item( newRowId - 1, SubitemColumn::Name );

    if( newRowId == 0 || (lastItem != nullptr && !lastItem->text().isEmpty()) )
    {
        this->twSubitems->insertRow( newRowId );
        this->twSubitems->setItem( newRowId, SubitemColumn::Name, new QTableWidgetItem( "" ) );
        this->twSubitems->setItem( newRowId, SubitemColumn::CategoryId, new QTableWidgetItem( this->cbCategoryOutgoing->currentText() ) );
        this->twSubitems->setItem( newRowId, SubitemColumn::Amount, new QTableWidgetItem( "0" ) );
        this->twSubitems->editItem( this->twSubitems->item( newRowId, SubitemColumn::Name ) );
    }
    else
    {
        this->twSubitems->editItem( this->twSubitems->item( newRowId - 1, SubitemColumn::Name ) );
    }
}


void EditTransactionDialog::deleteSubitem()
{
    int selectedRow = this->twSubitems->selectionModel()->selectedRows().first().row();
    this->twSubitems->removeRow( selectedRow );
}


void EditTransactionDialog::recalculateSubitemsAmount( int /* changedRow */, int changedColumn )
{
    if( changedColumn == SubitemColumn::Amount )
    {
        double newTotalAmount = 0.0;

        for( int row = 0; row < this->twSubitems->rowCount(); ++row )
        {
            newTotalAmount += this->twSubitems->item( row, SubitemColumn::Amount )->text().toDouble();
        }

        this->sbAmountOutgoing->setValue( newTotalAmount );
    }
}


void EditTransactionDialog::setupSubitemsWidget()
{
    this->twSubitems->setColumnWidth( SubitemColumn::Name, 100 );
    this->twSubitems->setColumnWidth( SubitemColumn::CategoryId, 110 );

    this->subitemCategoryDelegare = new ComboBoxDelegate();
    this->twSubitems->setItemDelegateForColumn( SubitemColumn::CategoryId, this->subitemCategoryDelegare );

    this->subitemAmountDelegate = new DoubleSpinboxDelegate();
    this->twSubitems->setItemDelegateForColumn( SubitemColumn::Amount, this->subitemAmountDelegate );

    this->connect( this->bSubitemAdd, &QPushButton::clicked, this, &EditTransactionDialog::createSubitem );
    this->connect( this->bSubitemDelete, &QPushButton::clicked, this, &EditTransactionDialog::deleteSubitem );

    this->connect( this->twSubitems, &QTableWidget::cellChanged, this, &EditTransactionDialog::recalculateSubitemsAmount );
}


int EditTransactionDialog::getCurrentType() const
{
    QAbstractButton* button = this->typesButtonGroup->checkedButton();

    if( button->objectName() == "bTypeOutgoing" )
    {
        return( TransactionsModel::Type::OUTGOING );
    }
    else if( button->objectName() == "bTypeIncoming" )
    {
        return( TransactionsModel::Type::INCOMING );
    }
    else // if( button->objectName() == "bTypeInternal" )
    {
        return( TransactionsModel::Type::INTERNAL );
    }
}


UniMap EditTransactionDialog::getCurrentPageValues() const
{
    UniMap values;

    switch( this-> getCurrentType() )
    {
        case TransactionsModel::Type::OUTGOING :
        {
            values.insert( "source_account_id", this->cbAccountOutgoing->currentData().toInt() );
            values.insert( "amount", this->sbAmountOutgoing->value() );
            values.insert( "category_id", this->cbCategoryOutgoing->currentData() );
            values.insert( "notes", this->eNotesOutgoing->toPlainText() );

            QList<QVariantList> subitems;

            for( int row = 0; row < this->twSubitems->rowCount(); ++row )
            {
                QVariantList rowData;

                // 0
                rowData.append( this->twSubitems->item( row, SubitemColumn::Name )->text() );
                // 1
                rowData.append( this->twSubitems->item( row, SubitemColumn::CategoryId )->data( Qt::UserRole ) );
                // 2
                rowData.append( this->twSubitems->item( row, SubitemColumn::Amount )->text() );

                subitems.append( rowData );
            }

            values.insert( "transaction_subitems", QVariant::fromValue( subitems ) );

            break;
        }

        case TransactionsModel::Type::INCOMING :
        {
            values.insert( "destination_account_id", this->cbDestinationAccountIncoming->currentData().toInt() );
            values.insert( "amount", this->sbAmountIncoming->value() );
            values.insert( "category_id", this->cbCategoryIncoming->currentData().toInt() );
            values.insert( "notes", this->eNotesIncoming->toPlainText() );

            break;
        }

        case TransactionsModel::Type::INTERNAL :
        {
            values.insert( "source_account_id", this->cbSourceAccountInternal->currentData().toInt() );
            values.insert( "destination_account_id", this->cbDestinationAccountInternal->currentData().toInt() );
            values.insert( "amount", this->sbAmountInternal->value() );
            values.insert( "category_id", 1 );
            values.insert( "notes", this->eNotesInternal->toPlainText() );

            break;
        }
    }

    return( values );
}


void EditTransactionDialog::okClicked()
{
    UniMap currentPageValues = this->getCurrentPageValues();

    UniMap fieldsData = {
        { "id",                     this->transactionId },
        { "type",                   this->getCurrentType() },
        { "amount",                 currentPageValues.value( "amount" ) },
        { "date",                   this->eDatetime->dateTime().toString( Qt::ISODate ) },
        { "source_account_id",      currentPageValues.value( "source_account_id", 0 ) },
        { "destination_account_id", currentPageValues.value( "destination_account_id", 0 ) },
        { "category_id",            currentPageValues.value( "category_id" ) },
        { "notes",                  currentPageValues.value( "notes" ) }
    };

    if( currentPageValues.contains( "transaction_subitems" ) )
    {
        fieldsData.insert( "transaction_subitems", currentPageValues.value( "transaction_subitems" ) );
    }

    emit this->saveData( fieldsData );

    this->accept();
}