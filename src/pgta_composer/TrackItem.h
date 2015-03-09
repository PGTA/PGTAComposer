
#pragma once

#include <QVariant>
#include <QUuid>
#include <QVector>
#include <QList>

class TrackItem
{

public:
    explicit TrackItem(const QVector<QVariant> &data, TrackItem *parent = nullptr,
                       const QUuid &uuid = QUuid(), const bool isGroup = false);
    ~TrackItem();

public:
    void AddChild(TrackItem *item);
    bool InsertChildren(int position, int count, int columns);
    bool RemoveChildren(int position, int count);
    TrackItem *GetChild(int row) const;
    int ChildCount() const;
    int ColumnCount() const;
    QVariant GetData(int column) const;
    bool SetData(int column, const QVariant &value);
    int GetRow() const;
    int GetChildRow(TrackItem *child) const;
    TrackItem *GetParent() const;
    bool IsGroup() const;
    void SetIsGroup(bool isGroup);
    QUuid GetUuid() const;

private:
    QList<TrackItem*> m_childItems;
    QVector<QVariant> m_itemData;
    QUuid m_uuid;
    TrackItem *m_parent;
    bool m_isGroup;
};
