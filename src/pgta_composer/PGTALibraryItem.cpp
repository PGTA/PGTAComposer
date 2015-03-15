#include "PGTALibraryItem.h"

PGTALibraryItem::PGTALibraryItem(const QVector<QVariant> &data, PGTALibraryItem *parent, const bool isFolder) :
    m_itemData(data),
    m_parent(parent),
    m_isFolder(isFolder)
{
}

PGTALibraryItem::~PGTALibraryItem()
{
    qDeleteAll(m_childItems);
}

bool PGTALibraryItem::IsFolder() const
{
    return m_isFolder;
}

PGTALibraryItem* PGTALibraryItem::GetParent() const
{
    return m_parent;
}

void PGTALibraryItem::SetIsFolder(bool isFolder)
{
    m_isFolder = isFolder;
}

int PGTALibraryItem::ChildCount() const
{
    return m_childItems.count();
}

void PGTALibraryItem::AddChild(PGTALibraryItem *item)
{
    m_childItems.append(item);
}

int PGTALibraryItem::GetChildRow(PGTALibraryItem *child) const
{
    if (child != nullptr)
    {
        return m_childItems.indexOf(child);
    }
    return -1;
}

PGTALibraryItem* PGTALibraryItem::GetChild(int row) const
{
    return m_childItems.value(row);
}

bool PGTALibraryItem::InsertChildren(int position, int count, int columns)
{
    if (!m_isFolder || position < 0 || position > m_childItems.size())
    {
        return false;
    }

    for (int i = 0; i < count; ++i)
    {
        QVector<QVariant> data(columns);
        PGTALibraryItem *item = new PGTALibraryItem(data, this);
        m_childItems.insert(position, item);
    }
    return true;
}

bool PGTALibraryItem::RemoveChildren(int position, int count)
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

int PGTALibraryItem::GetRow() const
{
    if (m_parent != nullptr)
    {
        return m_parent->GetChildRow(const_cast<PGTALibraryItem*>(this));
    }
    return -1;
}

int PGTALibraryItem::ColumnCount() const
{
    return m_itemData.count();
}

QVariant PGTALibraryItem::GetData(int column) const
{
    return m_itemData.value(column);
}

bool PGTALibraryItem::SetData(int column, const QVariant &value)
{
    if (column < 0 || column >= m_itemData.size())
    {
        return false;
    }
    m_itemData[column] = value;
    return true;
}
