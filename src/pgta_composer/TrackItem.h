
#pragma once

#include <QList>
#include <QVariant>

class TrackItem
{
public:
    TrackItem(const QList<QVariant> &data, TrackItem *parent = nullptr);
    ~TrackItem();

    void AddChild(TrackItem *item);

    TrackItem *GetChild(int row) const;
    int ChildCount() const;
    int ColumnCount() const;
    QVariant GetData(int column) const;
    int GetRow() const;
    int GetChildRow(TrackItem *child) const;
    TrackItem *GetParent() const;

private:
    QList<TrackItem*> m_childItems;
    QList<QVariant> m_itemData;
    TrackItem *m_parent;
};
