
#include "TrackItem.h"

TrackItem::TrackItem(const QVector<QVariant> &data, TrackItem *parent, bool isGroup) :
    m_isGroup(isGroup)
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

bool TrackItem::InsertChildren(int position, int count, int columns)
{
    if (!m_isGroup || position < 0 || position > m_childItems.size())
    {
        return false;
    }

    for (int i = 0; i < count; ++i)
    {
        QVector<QVariant> data(columns);
        TrackItem *item = new TrackItem(data, this);
        m_childItems.insert(position, item);
    }
    return true;
}

bool TrackItem::RemoveChildren(int position, int count)
{
    if (!m_isGroup || position < 0 || position + count > m_childItems.size())
    {
        return false;
    }

    for (int i = 0; i < count; ++i)
    {
        delete m_childItems.takeAt(position);
    }
    return true;
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

bool TrackItem::SetData(int column, const QVariant &value)
{
    if (column < 0 || column >= m_itemData.size())
    {
        return false;
    }
    m_itemData[column] = value;
    return true;
}

int TrackItem::GetRow() const
{
    if (m_parent != nullptr)
    {
        return m_parent->GetChildRow(const_cast<TrackItem*>(this));
    }
    return 0;
}

int TrackItem::GetChildRow(TrackItem *child) const
{
    if (child != nullptr)
    {
        return m_childItems.indexOf(child);
    }
    return 0;
}

TrackItem *TrackItem::GetParent() const
{
    return m_parent;
}

bool TrackItem::IsGroup() const
{
    return m_isGroup;
}
