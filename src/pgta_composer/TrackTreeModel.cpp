
#include <fstream>
#include <iostream>
#include "TrackTreeModel.h"
#include "TrackItem.h"

TrackTreeModel::TrackTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    QVector<QVariant> rootData;
    rootData << "Sample Name" << "Default File" << "Start Time" << "Frequency" << "Probability"
             << "Volume Multiplier" << "UUID";
    m_rootItem = new TrackItem(rootData);
}

TrackTreeModel::~TrackTreeModel()
{
    delete m_rootItem;
}

TrackItem *TrackTreeModel::getItem(const QModelIndex &index) const
{
    if (index.isValid())
    {
        TrackItem *item = static_cast<TrackItem*>(index.internalPointer());
        if (item)
        {
            return item;
        }
    }
    return m_rootItem;
}

QVariant TrackTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    if (role != Qt::DisplayRole && role != Qt::EditRole)
    {
        return QVariant();
    }

    TrackItem *item = static_cast<TrackItem*>(index.internalPointer());

    return item->GetData(index.column());
}

Qt::ItemFlags TrackTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return 0;
    }
    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant TrackTreeModel::headerData(int section, Qt::Orientation orientation,
                       int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        return m_rootItem->GetData(section);
    }

    return QVariant();
}

QModelIndex TrackTreeModel::index(int row, int column,
                     const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
    {
        return QModelIndex();
    }

    TrackItem *parentItem;

    if(!parent.isValid())
    {
        parentItem = m_rootItem;
    }
    else
    {
        parentItem = static_cast<TrackItem*>(parent.internalPointer());
    }

    TrackItem *childItem = parentItem->GetChild(row);

    if (childItem)
    {
        return createIndex(row, column, childItem);
    }
    else
    {
        return QModelIndex();
    }
}

QModelIndex TrackTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return QModelIndex();
    }

    TrackItem *childItem = static_cast<TrackItem*>(index.internalPointer());
    TrackItem *parentItem = childItem->GetParent();

    if (parentItem == m_rootItem)
    {
        return QModelIndex();
    }

    return createIndex(parentItem->GetRow(), 0, parentItem);
}

int TrackTreeModel::rowCount(const QModelIndex &parent) const
{
    TrackItem *parentItem;

    if (parent.column() > 0)
    {
        return 0;
    }

    if (!parent.isValid())
    {
        parentItem = m_rootItem;
    }
    else
    {
        parentItem = static_cast<TrackItem*>(parent.internalPointer());
    }

    return parentItem->ChildCount();
}

int TrackTreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return static_cast<TrackItem*>(parent.internalPointer())->ColumnCount();
    }
    else
    {
        return m_rootItem->ColumnCount();
    }
}

bool TrackTreeModel::setData(const QModelIndex &index, const QVariant &value,
             int role)
{
    if (role != Qt::EditRole)
    {
        return false;
    }

    TrackItem *item = getItem(index);
    bool retVal = item->SetData(index.column(), value);

    if (retVal)
    {
        emit dataChanged(index, index);
    }

    return retVal;
}

bool TrackTreeModel::setHeaderData(int section, Qt::Orientation orientation,
                   const QVariant &value, int role)
{
    if (role != Qt::EditRole || orientation != Qt::Horizontal)
    {
        return false;
    }

    bool retVal = m_rootItem->SetData(section, value);

    if (retVal)
    {
        emit headerDataChanged(orientation, section, section);
    }

    return retVal;
}

bool TrackTreeModel::insertRows(int row, int count, const QModelIndex &parent)
{
    TrackItem *parentItem = getItem(parent);
    bool retVal = true;
    beginInsertRows(parent, row, row + count - 1);
    retVal = parentItem->InsertChildren(row, count, m_rootItem->ColumnCount());
    endInsertRows();
    return retVal;
}

bool TrackTreeModel::removeRows(int row, int count, const QModelIndex &parent)
{
    TrackItem *parentItem = getItem(parent);
    bool retVal = true;
    beginRemoveRows(parent, row, row + count - 1);
    retVal = parentItem->RemoveChildren(row, count);
    endRemoveRows();
    return retVal;
}

void TrackTreeModel::addSample(const QVector<QVariant> &data, const QUuid &uuid)
{
    TrackItem *parent = getGroup(uuid);
    TrackItem *item = new TrackItem(data, parent);
    parent->AddChild(item);
}

void TrackTreeModel::addGroup(const QVector<QVariant> &data, const QUuid &uuid)
{
    if (uuid.isNull())
    {
        return;
    }

    TrackItem * parent = getGroup(uuid);
    if (parent != m_rootItem)
    {
        // Group already exists
        return;
    }
    TrackItem *item = new TrackItem(data, parent);
    m_groups.insert(uuid, item);
    parent->AddChild(item);
}

TrackItem* TrackTreeModel::getGroup(const QUuid &uuid) const
{
    if (!uuid.isNull())
    {
        auto groupItr = m_groups.find(uuid);
        if(groupItr!=m_groups.end())
        {
            return groupItr.value();
        }
    }
    return m_rootItem;
}
