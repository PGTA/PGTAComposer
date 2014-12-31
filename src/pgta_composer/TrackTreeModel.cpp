
#include <fstream>
#include <iostream>
#include "TrackTreeModel.h"
#include "TrackItem.h"
#include "Track.pb.h"

TrackTreeModel::TrackTreeModel(const QString &filePath, QObject *parent)
    : QAbstractItemModel(parent), m_filePath(filePath)
{
    QVector<QVariant> rootData;
    rootData << "Sample Name" << "File Path" << "Frequency" << "Probability"
             << "Volume Multiplier" << "Start Time";
    m_rootItem = new TrackItem(rootData);
    SetupModelData(m_rootItem);
}

TrackTreeModel::~TrackTreeModel()
{
    delete m_rootItem;
}

QVariant TrackTreeModel::data(const QModelIndex &index, int role) const
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

void TrackTreeModel::SetupModelData(TrackItem *parent)
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    PGTA::Track track;

    std::fstream protoInput(m_filePath.toStdString().c_str(), std::ios::in | std::ios::binary);

    if (!protoInput)
    {
        // TODO : error codes for string table lookup
        std::cout << "Failed to open input stream." << std::endl;
        return; // failed to open input stream
    }

    bool isParsed = track.ParseFromIstream(&protoInput);
    protoInput.close();

    if (!isParsed)
    {
        // TODO : error codes for string table lookup
        std::cout << "Failed to parse proto file." << std::endl;
        return; // failed to parse proto file
    }

    for (int i = 0; i < track.samples_size(); ++i)
    {
        const PGTA::Track_Sample &trackSample = track.samples(i);

        QVector<QVariant> data;
        data.append(QString::fromStdString("Sample " + std::to_string(i + 1)));
        data.append(QString::fromStdString(trackSample.filepath()));
        data.append(trackSample.frequency());
        data.append(trackSample.probability());
        data.append(trackSample.volumemultiplier());
        data.append(trackSample.starttime());

        TrackItem *item = new TrackItem(data, parent);

        parent->AddChild(item);
        parent = item;
    }
}
