/*************************************************************************************************
 *                                                                                                *
 *  file: doublespinboxdelegate.h                                                                 *
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

#ifndef DOUBLE_SPINBOX_DELEGATE_H
#define DOUBLE_SPINBOX_DELEGATE_H

#include <QStyledItemDelegate>


class DoubleSpinboxDelegate : public QStyledItemDelegate
{
    Q_OBJECT

    public:
        explicit DoubleSpinboxDelegate( QObject* parent = nullptr ) : QStyledItemDelegate( parent ) {}

        QWidget* createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

        void setEditorData( QWidget* editor, const QModelIndex& index ) const override;
        void setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const override;

        void updateEditorGeometry( QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index ) const override;
};


#endif // DOUBLE_SPINBOX_DELEGATE_H
