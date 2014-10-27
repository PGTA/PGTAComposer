#include "tracktablemodel.h"
#include "enginetrack.h"

TrackTableModel::TrackTableModel(QObject *parent) :
    QAbstractTableModel(parent),
    m_input(0)
{
}

void TrackTableModel::setInput(EngineTrack *input)
{
    Q_ASSERT(input);
    // emit approptiate signals so view knows model is updating
    beginResetModel();
    m_input = input;
    endResetModel();
}

int TrackTableModel::columnCount ( const QModelIndex & parent ) const
{
    if (!parent.isValid() || !m_input)
    {
        return 0;
    }
    return 7;
}

int	TrackTableModel::rowCount ( const QModelIndex & parent ) const
{
    if (!parent.isValid() || !m_input)
    {
        return 0;
    }
    return m_input->samples().count();
}

QVariant TrackTableModel::data ( const QModelIndex & index, int role ) const
{
    if (!index.isValid() || !m_input) {
        return QVariant();
    }
    QString sampleName = m_input->samples().at(index.row());
    switch (role) {
        case Qt::DisplayRole:
            switch (index.column()) {
                case EngineTrack::SampleName:
                    return sampleName;
                case EngineTrack::GroupName:
                    return m_input->group(sampleName);
                case EngineTrack::SampleFrequency:
                    return m_input->frequency(sampleName);
                case EngineTrack::SampleProbability:
                    return m_input->probability(sampleName);
                case EngineTrack::SampleVolMult:
                    return m_input->volumeMultiplier(sampleName);
                case EngineTrack::SampleStartTime:
                    return m_input->startTime(sampleName);
                default:
                    break;
            }
        default:
            break;
    }
    return QVariant();
}

QVariant TrackTableModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
    return QVariant();
}
