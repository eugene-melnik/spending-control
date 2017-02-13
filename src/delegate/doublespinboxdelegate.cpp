/*************************************************************************************************
 *                                                                                                *
 *  file: doublespinboxdelegate.cpp                                                               *
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

#include "doublespinboxdelegate.h"

#include <QDoubleSpinBox>


QWidget* DoubleSpinboxDelegate::createEditor( QWidget* parent, const QStyleOptionViewItem&, const QModelIndex& ) const
{
    QDoubleSpinBox* editor = new QDoubleSpinBox( parent );

    editor->setFrame( false );
    editor->setRange( 0.0, 10000000.0 );
    editor->setDecimals( 2 );

    return( editor );
}


void DoubleSpinboxDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const
{
    QDoubleSpinBox* spinBox = static_cast<QDoubleSpinBox*>( editor );
    spinBox->setValue( index.model()->data( index, Qt::EditRole ).toDouble() );
}


void DoubleSpinboxDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const
{
    QDoubleSpinBox* spinBox = static_cast<QDoubleSpinBox*>( editor );
    model->setData( index, spinBox->value(), Qt::EditRole );
}


void DoubleSpinboxDelegate::updateEditorGeometry( QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& ) const
{
    editor->setGeometry( option.rect );
}
