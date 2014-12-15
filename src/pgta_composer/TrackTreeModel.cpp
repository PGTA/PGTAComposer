
#include "TrackTreeModel.h"

TrackTreeModel::TrackTreeModel(const QString &data, QObject *parent)
    : QAbstractItemModel(parent)
{

}

TrackTreeModel::~TrackTreeModel()
{
    delete m_rootItem;
}

QVariant TrackTreeModel::GetData(const QModelIndex &index, int role) const
{

}

Qt::ItemFlags TrackTreeModel::GetFlags(const QModelIndex &index) const
{

}

QVariant TrackTreeModel::GetHeaderData(int section, Qt::Orientation orientation,
                       int role) const
{

}

QModelIndex TrackTreeModel::GetIndex(int row, int column,
                     const QModelIndex &parent) const
{

}
QModelIndex TrackTreeModel::GetParent(const QModelIndex &index) const
{

}

int TrackTreeModel::GetRowCount(const QModelIndex &parent) const
{

}

int TrackTreeModel::GetColumnCount(const QModelIndex &parent) const
{

}

void TrackTreeModel::SetupModelData(const QStringList &lines, TrackItem *parent)
{

}
