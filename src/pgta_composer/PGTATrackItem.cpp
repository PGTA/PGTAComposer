
#include <QUuid>
#include "PGTATrackItem.h"

PGTATrackItem::PGTATrackItem(const QVector<QVariant> &data, PGTATrackItem *parent, const QUuid &uuid, const bool isGroup) :
    m_itemData(data),
    m_uuid(uuid),
    m_parent(parent),
    m_isGroup(isGroup)
{
}

PGTATrackItem::~PGTATrackItem()
{
    qDeleteAll(m_childItems);
}

void PGTATrackItem::AddChild(PGTATrackItem *item)
{
    m_childItems.append(item);
}

bool PGTATrackItem::InsertChildren(int position, int count, int columns)
{
    if (!m_isGroup || position < 0 || position > m_childItems.size())
    {
        return false;
    }

    for (int i = 0; i < count; ++i)
    {
        QVector<QVariant> data(columns);
        PGTATrackItem *item = new PGTATrackItem(data, this, m_uuid);
        m_childItems.insert(position, item);
    }
    return true;
}

bool PGTATrackItem::RemoveChildren(int position, int count)
{
    if (position < 0 || position + count > m_childItems.size())
    {
        return false;
    }

    for (int i = 0; i < count; ++i)
    {
        delete m_childItems.takeAt(position);
    }
    return true;
}

PGTATrackItem *PGTATrackItem::GetChild(int row) const
{
    return m_childItems.value(row);
}

int PGTATrackItem::ChildCount() const
{
    return m_childItems.count();
}

int PGTATrackItem::ColumnCount() const
{
    return m_itemData.count();
}

QVariant PGTATrackItem::GetData(int column) const
{
    return m_itemData.value(column);
}

bool PGTATrackItem::SetData(int column, const QVariant &value)
{
    if (column < 0 || column >= m_itemData.size())
    {
        return false;
    }
    m_itemData[column] = value;
    return true;
}

int PGTATrackItem::GetRow() const
{
    if (m_parent != nullptr)
    {
        return m_parent->GetChildRow(const_cast<PGTATrackItem*>(this));
    }
    return -1;
}

int PGTATrackItem::GetChildRow(PGTATrackItem *child) const
{
    if (child != nullptr)
    {
        return m_childItems.indexOf(child);
    }
    return -1;
}

PGTATrackItem *PGTATrackItem::GetParent() const
{
    return m_parent;
}

bool PGTATrackItem::IsGroup() const
{
    return m_isGroup;
}

void PGTATrackItem::SetIsGroup(bool isGroup)
{
    m_uuid = QUuid::createUuid();
    m_isGroup = isGroup;
}

QUuid PGTATrackItem::GetUuid() const
{
    return m_uuid;
}
