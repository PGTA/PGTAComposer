
#include <fstream>
#include <iostream>
#include <QMimeData>
#include "TrackTreeModel.h"
#include "TrackItem.h"

TrackTreeModel::TrackTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    QVector<QVariant> rootData;
    rootData << "Sample Name" << "Default File" << "Start Time" << "Frequency" << "Probability"
             << "Volume Multiplier" << "UUID";
    m_rootItem = new TrackItem(rootData, nullptr, true);
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
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index) |  Qt::ItemIsEditable |
            Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if (index.isValid())
    {
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
    }
    else
    {
        return Qt::ItemIsDropEnabled | defaultFlags;
    }
}

Qt::DropActions TrackTreeModel::supportedDropActions () const
{
    return Qt::MoveAction | Qt::CopyAction;
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


QStringList TrackTreeModel::mimeTypes() const
 {
     QStringList types;
     types << "application/vnd.text.list";
     return types;
 }

QMimeData *TrackTreeModel::mimeData(const QModelIndexList &indexes) const
{
    if (!indexes.isEmpty() && isGroup(indexes.at(0)))
    {
        return nullptr;
    }
    QMimeData *mimeData = new QMimeData();

    mimeData.

    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    foreach(QModelIndex index, indexes)
    {
        if (index.isValid())
        {
            QString text = data(index, Qt::DisplayRole).toString();
            stream << text;
        }
    }
    mimeData->setData("application/vnd.text.list", encodedData);
    return mimeData;
}

bool TrackTreeModel::dropMimeData(const QMimeData * data, Qt::DropAction action,
                                        int row, int column, const QModelIndex & parent)
{
    QModelIndex newParent = parent;

    if (action == Qt::IgnoreAction)
    {
        return true;
    }

    if (!data)
    {
        return false;
    }

    if (!data->hasFormat("application/vnd.text.list"))
    {
        return false;
    }

    if (column > 0)
    {
        return false;
    }

    int beginRow = 0;

    if (row != -1)
    {
        beginRow = row;
    }
    else if (newParent.isValid() && isGroup(newParent))
    {
        beginRow = 0;
    }
    else
    {
        beginRow = rowCount(QModelIndex());
    }

    if (!isGroup(newParent))
    {
        newParent = QModelIndex();
    }

    // decode imported data
    QByteArray encodedData = data->data("application/vnd.text.list");
    QDataStream stream(&encodedData, QIODevice::ReadOnly);
    QStringList newItems;
    int cols = 0;

    while (!stream.atEnd())
    {
        QString text;
        stream >> text;
        newItems << text;
        ++cols;
    }

    int beginCols = 0;

    if (!insertRows(beginRow, 1, newParent))
    {
        return false;
    }

    foreach (QString text, newItems)
    {
        QModelIndex idx = index(beginRow, beginCols, newParent);
        setData(idx, text);
        beginCols++;
    }

    return true;
}



void TrackTreeModel::addSample(const QVector<QVariant> &data, const QUuid &uuid)
{
    TrackItem *parent = getGroup(uuid);
    TrackItem *item = new TrackItem(data, parent);
    parent->AddChild(item);
}

bool TrackTreeModel::isGroup(const QModelIndex &index) const
{
    TrackItem *item = getItem(index);
    return item->IsGroup();
}

void TrackTreeModel::setIsGroup(const QModelIndex &index) const
{
    TrackItem *item = getItem(index);
    item->SetIsGroup(true);
}

void TrackTreeModel::addGroup(const QVector<QVariant> &data, const QUuid &uuid)
{
    if (uuid.isNull())
    {
        return;
    }

    TrackItem * group = getGroup(uuid);
    if (group != m_rootItem)
    {
        // Group already exists
        return;
    }
    TrackItem *item = new TrackItem(data, group, true);
    m_groups.insert(uuid, item);
    group->AddChild(item);
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
