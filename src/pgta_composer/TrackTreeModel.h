
#pragma once

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

class TrackItem;

class TrackTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    TrackTreeModel(const QString &filePath, QObject *parent = nullptr);
    ~TrackTreeModel();

    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags GetFlags(const QModelIndex &index) const;
    QVariant GetHeaderData(int section, Qt::Orientation orientation,
                           int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                         const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

private:
    void SetupModelData(TrackItem *parent);
    TrackItem *m_rootItem;
    QString m_filePath;
};
