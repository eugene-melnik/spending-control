
#ifndef COMBO_BOX_DELEGATE_H
#define COMBO_BOX_DELEGATE_H

#include <QStyledItemDelegate>

#include "types.h"


class ComboBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT

    public:
        explicit ComboBoxDelegate( QObject* parent = nullptr ) : QStyledItemDelegate( parent ) {}

        void setValuesList( const UniMap& values );

        QWidget* createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

        void setEditorData( QWidget* editor, const QModelIndex& index ) const override;
        void setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const override;

        void updateEditorGeometry( QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index ) const override;

    protected:
        UniMap valuesList;
};


#endif // COMBO_BOX_DELEGATE_H
