
#ifndef ACCOUNTSLISTDIALOG_H
#define ACCOUNTSLISTDIALOG_H

#include <QAbstractItemModel>
#include <QDialog>

#include "ui_accountslistdialog.h"


class AccountsListDialog : public QDialog, protected Ui::AccountsListDialog
{
    Q_OBJECT

    public:
        explicit AccountsListDialog( QWidget* parent = nullptr );

        void setListModel( QAbstractItemModel* model );

        QAbstractItemModel* getListModel() const;

    signals:
        //

    protected:
        QAbstractItemModel* listModel = nullptr;
};

#endif // ACCOUNTSLISTDIALOG_H
