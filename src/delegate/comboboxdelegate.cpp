
#include "comboboxdelegate.h"

#include <QComboBox>


void ComboBoxDelegate::setValuesList( const UniMap& values )
{
    this->valuesList = values;
}


QWidget* ComboBoxDelegate::createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    QComboBox* editor = new QComboBox( parent );

    for( const QString& key : this->valuesList.keys() )
    {
        editor->addItem( this->valuesList.value( key ).toString(), key );
    }

    return( editor );
}


void ComboBoxDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const
{
    QComboBox* comboBox = static_cast<QComboBox*>( editor );
    comboBox->setCurrentText( index.model()->data( index, Qt::EditRole ).toString() );
}


void ComboBoxDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const
{
    QComboBox* comboBox = static_cast<QComboBox*>( editor );
    model->setData( index, comboBox->currentText(), Qt::EditRole );
    model->setData( index, comboBox->currentData(), Qt::UserRole );
}


void ComboBoxDelegate::updateEditorGeometry( QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& /* index */ ) const
{
    editor->setGeometry( option.rect );
}
