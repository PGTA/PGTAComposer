#include "tracktablemodel.h"
#include "enginetrack.h"

TrackTableModel::TrackTableModel(QObject *parent)
{

}

void TrackTableModel::setInput(EngineTrack *input)
{
    Q_ASSERT(input);
    m_input = input;
    //reset();
}

int TrackTableModel::columnCount ( const QModelIndex & parent ) const
{
    if (parent.isValid() || !m_input)
    {
        return 0;
    }
    return 7;
}

int	TrackTableModel::rowCount ( const QModelIndex & parent ) const
{
    if (parent.isValid() || !m_input)
    {
        return 0;
    }
    return m_input->samples().count();
}

QVariant TrackTableModel::data ( const QModelIndex & index, int role ) const
{
    return QVariant();
}

QVariant TrackTableModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
    return QVariant();
}
