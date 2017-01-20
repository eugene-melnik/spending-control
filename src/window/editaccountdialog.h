/*************************************************************************************************
 *                                                                                                *
 *  file: editaccountdialog.h                                                                     *
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

#ifndef EDIT_ACCOUNT_DIALOG_H
#define EDIT_ACCOUNT_DIALOG_H

#include <QDialog>
#include <QStringList>

#include "defines.h"
#include "ui_editaccountdialog.h"


class EditAccountDialog : public QDialog, protected Ui::EditAccountDialog
{
    Q_OBJECT

    public:
        explicit EditAccountDialog( QWidget* parent = nullptr );

        void setTypes( QStringList types );
        void setCurrencies( QStringList currencies );

        void setValues( const UniMap& fieldsData );

    signals:
        void saveData( UniMap fieldsData );

    protected:
        void okClicked();

        int recordId = 0;
};


#endif // EDIT_ACCOUNT_DIALOG_H
