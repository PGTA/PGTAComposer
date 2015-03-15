
#include <QIcon>
#include <QVariant>
#include <QModelIndex>
#include "PGTALibraryModel.h"

PGTALibraryModel::PGTALibraryModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    QVector<QVariant> rootData;
    rootData << "File Name" << "File Path" << "Duration";
    m_rootItem.reset(new PGTALibraryItem(rootData, nullptr));
}

PGTALibraryModel::~PGTALibraryModel()
{

}

QModelIndex PGTALibraryModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return QModelIndex();
    }

    PGTALibraryItem *childItem = static_cast<PGTALibraryItem*>(index.internalPointer());
    PGTALibraryItem *parentItem = childItem->GetParent();
    if (parentItem == m_rootItem.get())
    {
        return QModelIndex();
    }
    return createIndex(parentItem->GetRow(), 0, parentItem);
}

Qt::ItemFlags PGTALibraryModel::flags(const QModelIndex &index) const
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

QVariant PGTALibraryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    if (role == Qt::DecorationRole && index.column() == 0)
    {
        QVariant decoration;
        // TODO: need to have a different icon for different file types
        if (isFolder(index))
        {
            decoration.setValue(QIcon(""));
        }
        else
        {
            decoration.setValue(QIcon(""));
        }
    }

    if (role != Qt::DisplayRole && role != Qt::EditRole)
    {
        return QVariant();
    }
    PGTALibraryItem *item = static_cast<PGTALibraryItem*>(index.internalPointer());
    return item->GetData(index.column());

}

int PGTALibraryModel::rowCount(const QModelIndex &parent) const
{
    PGTALibraryItem *parentItem;
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
        parentItem = static_cast<PGTALibraryItem*>(parent.internalPointer());
    }
    return parentItem->ChildCount();
}

int PGTALibraryModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return static_cast<PGTALibraryItem*>(parent.internalPointer())->ColumnCount();
    }
    else
    {
        return m_rootItem->ColumnCount();
    }
}

bool PGTALibraryModel::insertRows(int row, int count, const QModelIndex &parent)
{
    PGTALibraryItem *parentItem = getItem(parent);
    bool retVal = false;
    beginInsertRows(parent, row, row + count -1);
    retVal = parentItem->InsertChildren(row, count, m_rootItem->ColumnCount());
    endInsertRows();
    return retVal;
}

bool PGTALibraryModel::removeRows(int row, int count, const QModelIndex &parent)
{
    PGTALibraryItem *parentItem = getItem(parent);
    bool retVal = false;
    beginRemoveRows(parent, row, row + count -1);
    retVal = parentItem->RemoveChildren(row, count);
    endRemoveRows();
    return retVal;
}

bool PGTALibraryModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole)
    {
        return false;
    }
    PGTALibraryItem *item = getItem(index);
    bool retVal = item->SetData(index.column(), value);
    if (retVal)
    {
        emit dataChanged(index, index);
    }
    return retVal;
}

QModelIndex PGTALibraryModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
    {
        return QModelIndex();
    }
    PGTALibraryItem *parentItem;
    if (!parent.isValid())
    {
        parentItem = m_rootItem.get();
    }
    else
    {
        parentItem = static_cast<PGTALibraryItem*>(parent.internalPointer());
    }
    PGTALibraryItem *childItem = parentItem->GetChild(row);
    if (childItem)
    {
        return createIndex(row, column, childItem);
    }
    else
    {
        return QModelIndex();
    }
}

QVariant PGTALibraryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        return m_rootItem->GetData(section);
    }
    return QVariant();
}

bool PGTALibraryModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
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

PGTALibraryItem* PGTALibraryModel::getItem(const QModelIndex &index) const
{
    if (index.isValid())
    {
        PGTALibraryItem *item = static_cast<PGTALibraryItem*>(index.internalPointer());
        if (item)
        {
            return item;
        }
    }
    return m_rootItem.get();
}

bool PGTALibraryModel::isFolder(const QModelIndex &index) const
{
    PGTALibraryItem *item = getItem(index);
    return item->IsFolder();
}
