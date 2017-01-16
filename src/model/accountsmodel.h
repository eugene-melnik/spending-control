
#ifndef ACCOUNTS_MODEL_H
#define ACCOUNTS_MODEL_H

#include <QAbstractTableModel>
#include <QMap>
#include <QStringList>
#include <QSqlDatabase>
#include <QVariantList>


class AccountsModel : public QAbstractTableModel
{
    Q_OBJECT

    public:
        AccountsModel( QSqlDatabase database );
        ~AccountsModel() {}

        int rowCount( const QModelIndex& parent = QModelIndex() ) const override;
        int columnCount( const QModelIndex& parent = QModelIndex() ) const override;

        QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const override;
        QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const override;

    protected:
        QVariantList getRecord( int row ) const;

        QSqlDatabase database;

        QStringList titles;
        mutable QMap<int,QVariantList> records;
};


#endif // ACCOUNTS_MODEL_H
