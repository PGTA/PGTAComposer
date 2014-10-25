#ifndef TRACKENGINE_H
#define TRACKENGINE_H

#include <QObject>
#include <QMap>
#include <QtCore/QStringList>
#include "Track.pb.h"

class Sample;

class TrackEngine : public QObject
{
    Q_OBJECT
public:
    explicit TrackEngine(QObject *parent = 0);

    ~TrackEngine();

    // initializes the TrackEngine object
    void init(const QString &fileName);

    // returns all sample names
    QStringList samples() const;

    // returns the file path by given sample name
    QString filePath(const QString &sampleName) const;

    // returns group name by given sample name
    QString group(const QString &sampleName) const;

    // returns frequency by given sample name
    qint64 frequency(const QString &sampleName) const;

    // returns probability by given sample name
    qint32 probability(const QString &sampleName) const;

    // returns vomume multiplier by given sample name
    qreal volumeMultiplier(const QString &sampleName) const;

    // returns vomume start time by given sample name
    qint64 startTime(const QString &sampleName) const;

    // returns all groups
    QStringList groups() const;

    // returns sample names filtered by group
    QStringList samplesByGroup(const QString &sampleName) const;

private:

    // populate TrackEngine object from disk (protobuf)
    void read();

    // write TrackEngie object to disk (protobuf)
    void write();

private:
    QString m_fileName;
    QMap<QString, Sample*> m_samples;
    QMultiMap<QString, QString> m_groups;
    PGTA::Track m_track;
};

#endif // TRACKENGINE_H
