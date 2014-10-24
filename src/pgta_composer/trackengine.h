#ifndef TRACKENGINE_H
#define TRACKENGINE_H

#include <QObject>
#include <QMap>
#include <QtCore/QStringList>


class Sample;

class TrackEngine : public QObject
{
    Q_OBJECT
public:
    explicit TrackEngine(QObject *parent = 0);

    ~TrackEngine();

    // returns all sample names
    QStringList samples() const;

    // returns group name by given sample name
    QString group(const QString &sampleName) const;

    // returns frequency by given sample name
    qint64 frequency(const QString &sampleName) const;

    // returns probability by given sample name
    qint32 probability(const QString &sampleName) const;

    // returns vomume multiplier by given sample name
    qreal volumeMultiplier(const QString &sampleName) const;

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
    QMap<QString, Sample*> m_samples;
    QMultiMap<QString, QString> m_groups;
};

#endif // TRACKENGINE_H
