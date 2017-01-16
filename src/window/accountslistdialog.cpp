
#include "accountslistdialog.h"


AccountsListDialog::AccountsListDialog( QWidget* parent )
  : QDialog( parent )
{
    this->setupUi( this );
}


void AccountsListDialog::setListModel( QAbstractItemModel* model )
{
    if( this->listModel != nullptr )
    {
        delete this->listModel;
    }

    this->listModel = model;
    this->lvAccounts->setModel( this->listModel );
}


QAbstractItemModel* AccountsListDialog::getListModel() const
{
    return( this->listModel );
}
