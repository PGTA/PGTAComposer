
#pragma once

#include <QtGui>

class EngineTrack;

class TrackTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TrackTableModel(QObject *parent = 0);
    void setInput(EngineTrack *input);
    int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    int	rowCount ( const QModelIndex & parent = QModelIndex() ) const;
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
private:
    EngineTrack *m_input;
};

