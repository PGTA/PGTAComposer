
#pragma once

#include <QList>
#include <QVector>
#include <QVariant>

class TrackItem
{
public:
    explicit TrackItem(const QVector<QVariant> &data, TrackItem *parent = nullptr,
                       bool isGroup = false);
    ~TrackItem();

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

private:
    QList<TrackItem*> m_childItems;
    QVector<QVariant> m_itemData;
    TrackItem *m_parent;
    bool m_isGroup;
};
