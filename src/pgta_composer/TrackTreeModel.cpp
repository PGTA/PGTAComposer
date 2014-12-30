
#include "TrackTreeModel.h"
#include "TrackItem.h"

TrackTreeModel::TrackTreeModel(const QString &data, QObject *parent)
    : QAbstractItemModel(parent)
{
    QList<QVariant> rootData;
    rootData << "";
    m_rootItem = new TrackItem(rootData);
    SetupModelData(data.split(QString("\n")), m_rootItem);
}

TrackTreeModel::~TrackTreeModel()
{
    delete m_rootItem;
}

QVariant TrackTreeModel::GetData(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    if (role != Qt::DisplayRole)
    {
        return QVariant();
    }

    TrackItem *item = static_cast<TrackItem*>(index.internalPointer());

    return item->GetData(index.column());
}

Qt::ItemFlags TrackTreeModel::GetFlags(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return 0;
    }
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant TrackTreeModel::GetHeaderData(int section, Qt::Orientation orientation,
                       int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        return m_rootItem->GetData(section);
    }

    return QVariant();
}

QModelIndex TrackTreeModel::GetIndex(int row, int column,
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

QModelIndex TrackTreeModel::GetParent(const QModelIndex &index) const
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

int TrackTreeModel::GetRowCount(const QModelIndex &parent) const
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

int TrackTreeModel::GetColumnCount(const QModelIndex &parent) const
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

void TrackTreeModel::SetupModelData(const QStringList &lines, TrackItem *parent)
{

}
