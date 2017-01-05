#pragma once

#include <memory>

#include <QAbstractItemModel>

namespace Quartz {

class PluginTableModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit PluginTableModel( QObject *parent );

    ~PluginTableModel();

    QModelIndex index( int row, int column, const QModelIndex &parent ) const;

    QModelIndex parent( const QModelIndex &child ) const;

    int rowCount( const QModelIndex &parent ) const;

    int columnCount( const QModelIndex &parent ) const;

    QVariant data( const QModelIndex &index, int role ) const;

private:
    struct Data;
    std::unique_ptr< Data > m_data;
};


}
