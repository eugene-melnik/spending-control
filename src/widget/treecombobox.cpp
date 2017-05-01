/*************************************************************************************************
 *                                                                                                *
 *  file: treecombobox.cpp                                                                        *
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

#include "treecombobox.h"

#include <QTreeView>


TreeComboBox::TreeComboBox( QWidget* parent )
  : QComboBox( parent )
{
    QTreeView* treeView = new QTreeView( this );
    treeView->setHeaderHidden( true );
    treeView->setItemsExpandable( false );
    treeView->setMinimumWidth( 250 );

    delete this->view();
    this->setView( treeView );
}


void TreeComboBox::setModel( QAbstractItemModel* model )
{
    QComboBox::setModel( model );

    this->setRootModelIndex( QModelIndex() );

    this->view()->selectionModel()->setCurrentIndex(
        model->index( 0, 0 ),
        QItemSelectionModel::SelectCurrent
    );
}


void TreeComboBox::hideColumn( int columnIndex )
{
    QTreeView* view = static_cast<QTreeView*>( this->view() );
    view->hideColumn( columnIndex );
}


QModelIndex TreeComboBox::currentIndex() const
{
    QTreeView* view = static_cast<QTreeView*>( this->view() );
    return( view->currentIndex() );
}


void TreeComboBox::showPopup()
{
    this->setRootModelIndex( QModelIndex() );

    QTreeView* view = static_cast<QTreeView*>( this->view() );
    view->expandAll();

    QComboBox::showPopup();
}
