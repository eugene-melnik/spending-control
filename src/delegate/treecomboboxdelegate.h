/*************************************************************************************************
 *                                                                                                *
 *  file: treecomboboxdelegate.h                                                                  *
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

#ifndef TREE_COMBO_BOX_DELEGATE_H
#define TREE_COMBO_BOX_DELEGATE_H

#include <QAbstractItemModel>
#include <QStyledItemDelegate>

#include "widget/treecombobox.h"
#include "types.h"


class TreeComboBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT

    public:
        explicit TreeComboBoxDelegate( QObject* parent = nullptr )
            : QStyledItemDelegate( parent ) {}

        void setModel( QAbstractItemModel* model )
        {
            if( this->model != nullptr )
            {
                delete this->model;
            }

            this->model = model;
        }

        void setDataColumn( int column )
        {
            this->dataColumn = column;
        }

        void hideColumns( const QList<int>& columns )
        {
            this->hiddenColumns = columns;
        }

        QWidget* createEditor( QWidget* parent, const QStyleOptionViewItem& /*option*/, const QModelIndex& /*index*/) const override
        {
            TreeComboBox* editor = new TreeComboBox( parent );
            editor->setModel( this->model );

            if( this->hiddenColumns.count() > 0)
            {
                for( int column : this->hiddenColumns )
                {
                    editor->hideColumn( column );
                }
            }

            return( editor );
        }

        void setEditorData( QWidget* editor, const QModelIndex& index ) const override
        {
            TreeComboBox* comboBox = static_cast<TreeComboBox*>( editor );
            QString a = index.data( Qt::EditRole ).toString();
            comboBox->setCurrentText( index.data( Qt::EditRole ).toString() );
        }

        void setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const override
        {
            TreeComboBox* comboBox = static_cast<TreeComboBox*>( editor );
            model->setData( index, comboBox->currentText(), Qt::EditRole );

            int selectedRow = comboBox->currentIndex().row();
            QModelIndex dataIndex = comboBox->model()->index( selectedRow, this->dataColumn );
            model->setData( index, comboBox->model()->data( dataIndex ), Qt::UserRole );
        }

        void updateEditorGeometry( QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& /*index*/ ) const override
        {
            editor->setGeometry( option.rect );
        }

    protected:
        QAbstractItemModel* model = nullptr;
        QList<int> hiddenColumns;
        int dataColumn = 0;
};


#endif // TREE_COMBO_BOX_DELEGATE_H
