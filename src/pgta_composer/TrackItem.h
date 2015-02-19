
#pragma once

#include <QList>
#include <QVector>
#include <QVariant>

class TrackItem
{
public:
    TrackItem(const QVector<QVariant> &data, TrackItem *parent = nullptr);
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

private:
    QList<TrackItem*> m_childItems;
    QVector<QVariant> m_itemData;
    TrackItem *m_parent;
};
