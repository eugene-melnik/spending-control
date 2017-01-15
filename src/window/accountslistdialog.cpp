
#include "accountslistdialog.h"


AccountsListDialog::AccountsListDialog( QWidget* parent )
  : QDialog( parent )
{
    this->setupUi( this );
}


void AccountsListDialog::setListModel( QAbstractItemModel* model )
{
    this->lvAccounts->setModel( model );
}
