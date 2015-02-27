
#pragma once

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QUuid>

class TrackItem;

class TrackTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum GroupColumn
    {
        GroupColumn_Name = 0,
        GroupColumn_UUID,
        GroupColumn_Restrictions
    };
    enum SampleColumn
    {
        SampleColumn_Name = 0,
        SampleColumn_FilePath,
        SampleColumn_StartTime,
        SampleColumn_Frequency,
        SampleColumn_Probability,
        SampleColumn_VolumeMultiplier,
        SampleColumn_GroupUUID,
        SampleColumn_Size
    };
public:
    TrackTreeModel(QObject *parent = nullptr);
    ~TrackTreeModel();

    void addSample(const QVector<QVariant> &data, const QUuid uuid);
    void addGroup(const QVector<QVariant> &data, const QUuid uuid);

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
    TrackItem *getGroup(const QUuid uuid) const;

private:
    TrackItem *m_rootItem;
};
