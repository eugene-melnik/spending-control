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

#include "model/categorytreeitem.h"
#include "model/transactionsmodel.h"
#include "edittransactiondialog.h"

#include <QDateTime>
#include <QMessageBox>


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


void EditTransactionDialog::setIncomeCategoriesModel( QAbstractItemModel* model )
{
    delete this->cbCategoryIncoming->model();

    this->cbCategoryIncoming->setModel( model );
    this->cbCategoryIncoming->hideColumn( CategoryTreeItem::Column::Description );
    this->cbCategoryIncoming->hideColumn( CategoryTreeItem::Column::Id );
    this->cbCategoryIncoming->hideColumn( CategoryTreeItem::Column::ParentCategoryId );
    this->cbCategoryIncoming->hideColumn( CategoryTreeItem::Column::Type );
}


void EditTransactionDialog::setOutcomeCategoriesModel( QAbstractItemModel* model )
{
    delete this->cbCategoryOutgoing->model();

    this->cbCategoryOutgoing->setModel( model );
    this->cbCategoryOutgoing->hideColumn( CategoryTreeItem::Column::Description );
    this->cbCategoryOutgoing->hideColumn( CategoryTreeItem::Column::Id );
    this->cbCategoryOutgoing->hideColumn( CategoryTreeItem::Column::ParentCategoryId );
    this->cbCategoryOutgoing->hideColumn( CategoryTreeItem::Column::Type );

    this->subitemCategoryDelegare->setModel( model );
    this->subitemCategoryDelegare->hideColumns({
        CategoryTreeItem::Column::Description,
        CategoryTreeItem::Column::Id,
        CategoryTreeItem::Column::ParentCategoryId,
        CategoryTreeItem::Column::Type
    });
}


void EditTransactionDialog::setValues( const UniMap& fieldsData )
{
    QString date = fieldsData.value( "date" ).toString();
    this->wCalendar->setSelectedDate( QDate::fromString( date , Qt::ISODate ) );
    this->eDatetime->setDateTime( QDateTime::fromString( date , Qt::ISODate ) );

    int type = fieldsData.value( "type" ).toInt();
    this->clearPageValues( type );

    switch( type )
    {
        case TransactionsModel::Outgoing :
        {
            this->bTypeOutgoing->setChecked( true );

            //

            break;
        }

        case TransactionsModel::Incoming :
        {
            this->bTypeIncoming->setChecked( true );

            //

            break;
        }

        case TransactionsModel::Internal :
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
    this->swContent->setCurrentIndex( this->getCurrentType() );
}


void EditTransactionDialog::clearPageValues( int page )
{
    this->setCurrentDate();

    switch( page )
    {
        case TransactionsModel::Outgoing :
        {
            this->cbAccountOutgoing->setCurrentIndex( 0 );
            this->sbAmountOutgoing->setValue( 0.0 );
            this->cbCategoryOutgoing->setCurrentIndex( 0 );
            this->eNotesOutgoing->clear();
            this->twSubitems->clearContents();
            this->twSubitems->setRowCount( 0 );
            break;
        }

        case TransactionsModel::Incoming :
        {
            this->cbDestinationAccountIncoming->setCurrentIndex( 0 );
            this->sbAmountIncoming->setValue( 0.0 );
            this->cbCategoryIncoming->setCurrentIndex( 0 );
            this->eNotesIncoming->clear();
            break;
        }

        case TransactionsModel::Internal :
        {
            this->cbSourceAccountInternal->setCurrentIndex( 0 );
            this->cbDestinationAccountInternal->setCurrentIndex( 1 );
            this->sbAmountInternal->setValue( 0.0 );
            this->eNotesInternal->clear();
            break;
        }
    }
}


void EditTransactionDialog::createSubitem()
{
    int newRowId = this->twSubitems->rowCount();

    QTableWidgetItem* lastItem = this->twSubitems->item( newRowId - 1, SubitemColumn::Name );

    if( newRowId == 0 || (lastItem != nullptr && !lastItem->text().isEmpty()) )
    {
        this->twSubitems->insertRow( newRowId );

        QTableWidgetItem* name = new QTableWidgetItem( "" );
        this->twSubitems->setItem( newRowId, SubitemColumn::Name, name );

        QTableWidgetItem* category = new QTableWidgetItem( this->cbCategoryOutgoing->currentText() );
        category->setData( Qt::UserRole, this->getCategoryId( this->cbCategoryOutgoing ) );
        this->twSubitems->setItem( newRowId, SubitemColumn::CategoryId, category );

        QTableWidgetItem* amount = new QTableWidgetItem( "0" );
        this->twSubitems->setItem( newRowId, SubitemColumn::Amount, amount );

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


void EditTransactionDialog::recalculateSubitemsAmount( int changedRow, int changedColumn )
{
    Q_UNUSED(changedRow)

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

    this->subitemCategoryDelegare = new TreeComboBoxDelegate();
    this->subitemCategoryDelegare->setDataColumn( CategoryTreeItem::Column::Id );
    this->twSubitems->setItemDelegateForColumn( SubitemColumn::CategoryId, this->subitemCategoryDelegare );

    this->subitemAmountDelegate = new DoubleSpinboxDelegate();
    this->twSubitems->setItemDelegateForColumn( SubitemColumn::Amount, this->subitemAmountDelegate );

    this->connect( this->bSubitemAdd, &QPushButton::clicked, this, &EditTransactionDialog::createSubitem );
    this->connect( this->bSubitemDelete, &QPushButton::clicked, this, &EditTransactionDialog::deleteSubitem );

    this->connect( this->twSubitems, &QTableWidget::cellChanged, this, &EditTransactionDialog::recalculateSubitemsAmount );
}


int EditTransactionDialog::getCategoryId( TreeComboBox* comboBox ) const
{
    QModelIndex currentIndex = comboBox->currentIndex();

    QModelIndex idIndex = comboBox->model()->index(
        currentIndex.row(), CategoryTreeItem::Column::Id, currentIndex.parent()
    );

    return( idIndex.data().toInt() );
}


int EditTransactionDialog::getCurrentType() const
{
    QAbstractButton* button = this->typesButtonGroup->checkedButton();

    if( button->objectName() == "bTypeOutgoing" )
    {
        return( TransactionsModel::Outgoing );
    }
    else if( button->objectName() == "bTypeIncoming" )
    {
        return( TransactionsModel::Incoming );
    }
    else // if( button->objectName() == "bTypeInternal" )
    {
        return( TransactionsModel::Internal );
    }
}


UniMap EditTransactionDialog::getCurrentPageValues() const
{
    UniMap values;

    switch( this-> getCurrentType() )
    {
        case TransactionsModel::Outgoing :
        {
            values.insert( "source_account_id", this->cbAccountOutgoing->currentData() );
            values.insert( "amount", this->sbAmountOutgoing->value() );
            values.insert( "notes", this->eNotesOutgoing->toPlainText() );
            values.insert( "category_id", this->getCategoryId( this->cbCategoryOutgoing ) );

            QList<QVariantList> subitems;

            for( int row = 0; row < this->twSubitems->rowCount(); ++row )
            {
                QVariantList rowData;

                // rowData[0]
                rowData.append( this->twSubitems->item( row, SubitemColumn::Name )->text() );
                // rowData[1]
                rowData.append( this->twSubitems->item( row, SubitemColumn::CategoryId )->data( Qt::UserRole ) );
                // rowData[2]
                rowData.append( (int) (this->twSubitems->item( row, SubitemColumn::Amount )->text().toDouble() * 100) );

                subitems.append( rowData );
            }

            values.insert( "transaction_subitems", QVariant::fromValue( subitems ) );

            break;
        }

        case TransactionsModel::Incoming :
        {
            values.insert( "destination_account_id", this->cbDestinationAccountIncoming->currentData() );
            values.insert( "amount", this->sbAmountIncoming->value() );
            values.insert( "category_id", this->cbCategoryIncoming->currentData() );
            values.insert( "notes", this->eNotesIncoming->toPlainText() );

            break;
        }

        case TransactionsModel::Internal :
        {
            values.insert( "source_account_id", this->cbSourceAccountInternal->currentData() );
            values.insert( "destination_account_id", this->cbDestinationAccountInternal->currentData() );
            values.insert( "amount", this->sbAmountInternal->value() );
            values.insert( "category_id", 1 ); // have no category
            values.insert( "notes", this->eNotesInternal->toPlainText() );

            break;
        }
    }

    return( values );
}


void EditTransactionDialog::okClicked()
{
    if( !this->isRequiredFieldsFilled() )
    {
        return;
    }

    UniMap currentPageValues = this->getCurrentPageValues();

    UniMap fieldsData = {
        { "id",                     this->transactionId },
        { "type",                   this->getCurrentType() },
        { "amount",                 (int) (currentPageValues.value( "amount" ).toDouble() * 100) },
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


bool EditTransactionDialog::isRequiredFieldsFilled()
{
    QString messageboxTitle = tr( "Save transaction data" );
    QString messageAmountTooLow = tr( "It seems like the total amount of the transaction is too low. Are you sure you want to save it?" );

    switch( this-> getCurrentType() )
    {
        case TransactionsModel::Outgoing :
        {
            // Transaction marked as 'has subitems'
            // but have not actually subitems
            if( this->cSubitems->isChecked() && this->twSubitems->rowCount() == 0 )
            {
                QMessageBox::information(
                    this, messageboxTitle,
                    tr( "Should contain at least one subitem!" )
                );

                return( false );
            }

            // Transaction without subitems
            // should have description (notes)
            if( this->eNotesOutgoing->toPlainText().isEmpty() && !this->cSubitems->isChecked() )
            {
                QMessageBox::information(
                    this, messageboxTitle,
                    tr( "Field \"Notes\" is required!" )
                );

                return( false );
            }

            // Ask if total amount is about zero
            // may be some mistake
            if( this->sbAmountOutgoing->value() < 0.01 )
            {
                int response = QMessageBox::question(
                    this, messageboxTitle,
                    messageAmountTooLow
                );

                if( response == QMessageBox::StandardButton::No
                    || response == QMessageBox::StandardButton::Cancel )
                {
                    return( false );
                }
            }

            break;
        }

        case TransactionsModel::Incoming :
        {
            // Ask if total amount is about zero
            // may be some mistake
            if( this->sbAmountIncoming->value() < 0.01 )
            {
                int response = QMessageBox::question(
                    this, messageboxTitle,
                    messageAmountTooLow
                );

                if( response == QMessageBox::StandardButton::No
                    || response == QMessageBox::StandardButton::Cancel )
                {
                    return( false );
                }
            }

            // Notes for the incoming transaction are not mandatory
            // but need to ask user if he left it empty
            if( this->eNotesIncoming->toPlainText().isEmpty() )
            {
                int response = QMessageBox::question(
                    this, messageboxTitle,
                    tr( "Are you sure you want to save transaction notes empty?" )
                );

                if( response == QMessageBox::StandardButton::No
                    || response == QMessageBox::StandardButton::Cancel )
                {
                    return( false );
                }
            }

            // TODO: add category != root checking (?)

            break;
        }

        case TransactionsModel::Internal :
        {
            // Ask if total amount is about zero
            // may be some mistake
            if( this->sbAmountInternal->value() < 0.01 )
            {
                int response = QMessageBox::question(
                    this, messageboxTitle,
                    messageAmountTooLow
                );

                if( response == QMessageBox::StandardButton::No
                    || response == QMessageBox::StandardButton::Cancel )
                {
                    return( false );
                }
            }

            if( this->cbSourceAccountInternal->currentData() == this->cbDestinationAccountInternal->currentData())
            {
                QMessageBox::information(
                    this, messageboxTitle,
                    tr( "\"Source\" and \"Destination\" accounts should not be the same!" )
                );

                return( false );
            }

            break;
        }
    }

    return( true );
}
