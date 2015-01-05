
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

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                           int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                         const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;
    bool setHeaderData(int section, Qt::Orientation orientation,
                       const QVariant &value, int role = Qt::EditRole) override;
    bool insertRows(int row, int count,
                    const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count,
                    const QModelIndex &parent = QModelIndex()) override;



private:
    TrackItem *getItem(const QModelIndex &index) const;
    void SetupModelData(TrackItem *parent);
    TrackItem *m_rootItem;
    QString m_filePath;
};
