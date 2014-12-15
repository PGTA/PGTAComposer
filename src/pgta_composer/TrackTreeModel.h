
#pragma once

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

class TrackItem;

class TrackTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    TrackTreeModel(const QString &data, QObject *parent = nullptr);
    ~TrackTreeModel();

    QVariant GetData(const QModelIndex &index, int role) const;
    Qt::ItemFlags GetFlags(const QModelIndex &index) const;
    QVariant GetHeaderData(int section, Qt::Orientation orientation,
                           int role = Qt::DisplayRole) const;
    QModelIndex GetIndex(int row, int column,
                         const QModelIndex &parent = QModelIndex()) const;
    QModelIndex GetParent(const QModelIndex &index) const;
    int GetRowCount(const QModelIndex &parent = QModelIndex()) const;
    int GetColumnCount(const QModelIndex &parent = QModelIndex()) const;

private:
    void SetupModelData(const QStringList &lines, TrackItem *parent);
    TrackItem *m_rootItem;
};
