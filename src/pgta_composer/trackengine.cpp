#include "trackengine.h"

class Sample
{
public:
    QString sampleName;
    QString groupName;
    qint64 frequency;
    qint32 probability;
    qreal volumeMultiplier;
};


TrackEngine::TrackEngine(QObject *parent) :
    QObject(parent)
{
}


TrackEngine::~TrackEngine()
{

}

QStringList TrackEngine::samples() const
{
    return m_samples.keys();
}

QString TrackEngine::group(const QString &sampleName) const
{
    Sample *sample = m_samples[sampleName];
    if (sample == nullptr)
    {
        return QString();
    }
    return sample->groupName;
}


qint64 TrackEngine::frequency(const QString &sampleName) const
{
    Sample *sample = m_samples[sampleName];
    if (sample == nullptr)
    {
        return -1;
    }
    return sample->frequency;
}

qint32 TrackEngine::probability(const QString &sampleName) const
{
    Sample *sample = m_samples[sampleName];
    if (sample == nullptr)
    {
        return -1;
    }
    return sample->probability;
}


qreal TrackEngine::volumeMultiplier(const QString &sampleName) const
{
    Sample *sample = m_samples[sampleName];
    if (sample == nullptr)
    {
        return -1;
    }
    return sample->volumeMultiplier;
}

QStringList TrackEngine::groups() const
{
   return m_groups.uniqueKeys();
}


QStringList TrackEngine::samplesByGroup(const QString &sampleName) const
{
    return m_groups.values(sampleName);
}

void TrackEngine::read()
{

}

void TrackEngine::write()
{

}
