
#include <QMimeData>
#include <QIcon>
#include <QVariant>
#include "PGTATrackTreeModel.h"
#include "PGTATrackItem.h"

PGTATrackTreeModel::PGTATrackTreeModel(QObject *parent)
    : QAbstractItemModel(parent),
      m_isDirty(false)
{
    QVector<QVariant> rootData;
    rootData << "Sample Name" << "Default File" << "Start Time" << "Period" << "Period Deviation" << "Probability"
             << "Gain" << "UUID";
    m_rootItem.reset(new PGTATrackItem(rootData, nullptr,  QUuid(), true));
}

PGTATrackTreeModel::~PGTATrackTreeModel()
{
}

PGTATrackItem *PGTATrackTreeModel::getItemSafe(const QModelIndex &index) const
{
    if (index.isValid())
    {
        PGTATrackItem *item = static_cast<PGTATrackItem*>(index.internalPointer());
        if (item)
        {
            return item;
        }
    }
    return m_rootItem.get();
}

PGTATrackItem *PGTATrackTreeModel::getItemUnsafe(const QModelIndex &index)
{
    if (index.isValid())
    {
        PGTATrackItem *item = static_cast<PGTATrackItem*>(index.internalPointer());
        if (item)
        {
            return item;
        }
    }
    return m_rootItem.get();
}

QVariant PGTATrackTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    if (role == Qt::DecorationRole && index.column() == 0)
    {
        QVariant decoration;
        if (isGroup(index))
        {
            decoration.setValue(QIcon(":/img/group_32x32.png"));
        }
        else
        {
            decoration.setValue(QIcon(":img/sample_32x32.png"));
        }
        return decoration;
    }

    if (role != Qt::DisplayRole && role != Qt::EditRole)
    {
        return QVariant();
    }
    PGTATrackItem *item = static_cast<PGTATrackItem*>(index.internalPointer());
    return item->GetData(index.column());
}

Qt::ItemFlags PGTATrackTreeModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index) | Qt::ItemIsEditable |
            Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if (index.isValid())
    {
        flags |= Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
    }
    else
    {
        flags |= Qt::ItemIsDropEnabled;
    }
    return flags;
}

Qt::DropActions PGTATrackTreeModel::supportedDropActions () const
{
    return Qt::MoveAction | Qt::CopyAction;
}

QVariant PGTATrackTreeModel::headerData(int section, Qt::Orientation orientation,
                       int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        return m_rootItem->GetData(section);
    }
    return QVariant();
}

QModelIndex PGTATrackTreeModel::index(int row, int column,
                     const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
    {
        return QModelIndex();
    }

    PGTATrackItem *parentItem;
    if (!parent.isValid())
    {
        parentItem = m_rootItem.get();
    }
    else
    {
        parentItem = static_cast<PGTATrackItem*>(parent.internalPointer());
    }

    PGTATrackItem *childItem = parentItem->GetChild(row);
    if (childItem)
    {
        return createIndex(row, column, childItem);
    }
    else
    {
        return QModelIndex();
    }
}

QModelIndex PGTATrackTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return QModelIndex();
    }

    PGTATrackItem *childItem = static_cast<PGTATrackItem*>(index.internalPointer());
    PGTATrackItem *parentItem = childItem->GetParent();
    if (parentItem == m_rootItem.get())
    {
        return QModelIndex();
    }
    return createIndex(parentItem->GetRow(), 0, parentItem);
}

int PGTATrackTreeModel::rowCount(const QModelIndex &parent) const
{
    PGTATrackItem *parentItem;
    if (parent.column() > 0)
    {
        return 0;
    }

    if (!parent.isValid())
    {
        parentItem = m_rootItem.get();
    }
    else
    {
        parentItem = static_cast<PGTATrackItem*>(parent.internalPointer());
    }
    return parentItem->ChildCount();
}

int PGTATrackTreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return static_cast<PGTATrackItem*>(parent.internalPointer())->ColumnCount();
    }
    else
    {
        return m_rootItem->ColumnCount();
    }
}

bool PGTATrackTreeModel::setData(const QModelIndex &index, const QVariant &value,
             int role)
{
    if (role != Qt::EditRole)
    {
        return false;
    }

    PGTATrackItem *item = getItemSafe(index);
    bool retVal = item->SetData(index.column(), value);
    if (retVal)
    {
        emit dataChanged(index, index);
        m_isDirty = true;
    }
    return retVal;
}

bool PGTATrackTreeModel::setHeaderData(int section, Qt::Orientation orientation,
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

bool PGTATrackTreeModel::insertRows(int row, int count, const QModelIndex &parent)
{
    PGTATrackItem *parentItem = getItemSafe(parent);
    bool retVal = true;
    beginInsertRows(parent, row, row + count - 1);
    retVal = parentItem->InsertChildren(row, count, m_rootItem->ColumnCount());
    endInsertRows();
    return retVal;
}

bool PGTATrackTreeModel::removeRows(int row, int count, const QModelIndex &parent)
{
    PGTATrackItem *parentItem = getItemSafe(parent);
    bool retVal = true;

    // determine if any children being remove are group
    std::vector<QUuid> uuids;
    for (int i = 0; i < count; ++i)
    {
        QModelIndex tmpIndex = index(row + i, 0, parent);
        if (isGroup(tmpIndex))
        {
            uuids.push_back(getUuid(tmpIndex));
        }
    }

    beginRemoveRows(parent, row, row + count - 1);
    retVal = parentItem->RemoveChildren(row, count);
    endRemoveRows();

    // remove all groups from the aux data structure
    for (auto &uuid : uuids)
    {
        removeGroup(uuid);
    }

    m_isDirty = true;
    return retVal;
}


QStringList PGTATrackTreeModel::mimeTypes() const
 {
     QStringList types;
     types << "application/vnd.text.list";
     return types;
 }

QMimeData *PGTATrackTreeModel::mimeData(const QModelIndexList &indices) const
{
    if (indices.isEmpty() || !indices.at(0).isValid() || isGroup(indices.at(0)))
    {
        return nullptr;
    }
    QModelIndex currIdx = indices.at(0);
    int numCols = columnCount();
    int row = currIdx.row();
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;
    QDataStream stream(&encodedData, QIODevice::WriteOnly);
    for (int i = 0; i < numCols; ++i)
    {
        QModelIndex idx = index(row, i, currIdx.parent());
        if (idx.isValid())
        {
            QString text = data(idx, Qt::DisplayRole).toString();
            stream << text;
        }
    }
    mimeData->setData("application/vnd.text.list", encodedData);
    return mimeData;
}

bool PGTATrackTreeModel::dropMimeData(const QMimeData * data, Qt::DropAction action,
                                        int row, int column, const QModelIndex & parent)
{
    if (action == Qt::IgnoreAction)
    {
        return true;
    }

    if (!data || !data->hasFormat("application/vnd.text.list") || column > 0)
    {
        return false;
    }

    // parent must be colum zero in order for drop to work properly
    QModelIndex destParent = index(parent.row(), 0, parent.parent());

    int targetRow = 0;
    // check to see if drop target is a group
    if (!isGroup(destParent))
    {
        targetRow = destParent.row() + 1;
        destParent = destParent.parent();
    }

    // set drop row
    if (row < 0)
    {
        if (destParent.isValid() && isGroup(destParent))
        {
            row = targetRow;
        }
        else
        {
            row = rowCount(QModelIndex());
        }
    }

    // check to see if parent is
    if (!isGroup(destParent))
    {
        destParent = QModelIndex();
    }

    // decode imported data
    QByteArray encodedData = data->data("application/vnd.text.list");
    QDataStream stream(&encodedData, QIODevice::ReadOnly);
    QStringList newItems;
    while (!stream.atEnd())
    {
        QString text;
        stream >> text;
        newItems << text;
    }

    // attempt to insert new row
    if (!insertRows(row, 1, destParent))
    {
        return false;
    }

    // set the data of the newly inserted row
    int beginCols = 0;
    for (auto &text : newItems)
    {
        QModelIndex idx = index(row, beginCols, destParent);

        if (beginCols == SampleColumn_GroupUUID)
        {
            setData(idx, getUuid(destParent));
        }
        else
        {
            setData(idx, text);
        }
        beginCols++;
    }
    return true;
}

void PGTATrackTreeModel::addSample(const QVector<QVariant> &data, const QUuid &uuid)
{
    PGTATrackItem *parent = getGroup(uuid);
    PGTATrackItem *item = new PGTATrackItem(data, parent);
    parent->AddChild(item);
}

bool PGTATrackTreeModel::isGroup(const QModelIndex &index) const
{
    PGTATrackItem *item = getItemSafe(index);
    return item->IsGroup();
}

void PGTATrackTreeModel::setIsGroup(const QModelIndex &index)
{
    PGTATrackItem *item = getItemUnsafe(index);
    item->SetIsGroup(true);
    m_groups.insert(item->GetUuid(), item);
}

void PGTATrackTreeModel::addGroup(const QVector<QVariant> &data, const QUuid &uuid)
{
    if (uuid.isNull())
    {
        return;
    }

    PGTATrackItem * group = getGroup(uuid);
    if (group != m_rootItem.get())
    {
        // Group already exists
        return;
    }
    PGTATrackItem *item = new PGTATrackItem(data, group, uuid, true);
    m_groups.insert(uuid, item);
    group->AddChild(item);
}

PGTATrackItem* PGTATrackTreeModel::getGroup(const QUuid &uuid) const
{
    if (!uuid.isNull())
    {
        auto groupItr = m_groups.find(uuid);
        if(groupItr!=m_groups.end())
        {
            return groupItr.value();
        }
    }
    return m_rootItem.get();
}

QUuid PGTATrackTreeModel::getUuid(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return QUuid();
    }
    PGTATrackItem *item = getItemSafe(index);
    return item->GetUuid();
}

void PGTATrackTreeModel::removeGroup(const QUuid &uuid)
{
    if(!uuid.isNull())
    {
        m_groups.remove(uuid);
    }
}

const PGTATrackItem *PGTATrackTreeModel::getRoot() const
{
    return m_rootItem.get();
}

void PGTATrackTreeModel::setFilePath(const QString &filePath)
{
    m_filePath = filePath;
}

QString PGTATrackTreeModel::getFilePath() const
{
    return m_filePath;
}

bool PGTATrackTreeModel::getIsDirty() const
{
    return m_isDirty;
}

void PGTATrackTreeModel::setIsDirty(bool isDirty)
{
    m_isDirty = isDirty;
}

