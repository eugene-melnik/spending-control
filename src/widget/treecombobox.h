/*************************************************************************************************
 *                                                                                                *
 *  file: treecombobox.h                                                                          *
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

#ifndef TREE_COMBO_BOX_H
#define TREE_COMBO_BOX_H

#include <QComboBox>


class TreeComboBox : public QComboBox
{
    Q_OBJECT

    public:
        explicit TreeComboBox( QWidget* parent = nullptr );

        void setModel( QAbstractItemModel* model );

        void hideColumn( int columnIndex );

        QModelIndex currentIndex() const;

    protected:
        void showPopup() override;
};


#endif // TREE_COMBO_BOX_H
