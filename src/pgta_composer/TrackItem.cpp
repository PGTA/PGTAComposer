
#include "TrackItem.h"

TrackItem::TrackItem(const QList<QVariant> &data, TrackItem *parent)
{
    m_parent = parent;
    m_itemData = data;
}

TrackItem::~TrackItem()
{
    qDeleteAll(m_childItems);
}

void TrackItem::AddChild(TrackItem *item)
{
    m_childItems.append(item);
}

TrackItem *TrackItem::GetChild(int row) const
{
    return m_childItems.value(row);
}

int TrackItem::ChildCount() const
{
    return m_childItems.count();
}

int TrackItem::ColumnCount() const
{
    return m_itemData.count();
}

QVariant TrackItem::GetData(int column) const
{
    return m_itemData.value(column);
}

int TrackItem::GetRow() const
{
    if (m_parent != nullptr)
    {
        return m_parent->GetChildRow(const_cast<TrackItem*>(this));
    }
}

int TrackItem::GetChildRow(TrackItem *child) const
{
    if (child != nullptr)
    {
        return m_childItems.indexOf(child);
    }
}

TrackItem *TrackItem::GetParent() const
{
    return m_parent;
}
