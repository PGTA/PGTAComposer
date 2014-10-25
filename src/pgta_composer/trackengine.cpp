
#include <iostream>
#include <fstream>
#include "trackengine.h"
#include "Track.pb.h"

class Sample
{
public:
    qint64 frequency;
    qint64 startTime;
    qint32 probability;
    qreal volumeMultiplier;
    QString sampleName;
    QString groupName;
    QString filePath;
};

TrackEngine::TrackEngine(QObject *parent) :
    QObject(parent)
{
}

TrackEngine::~TrackEngine()
{
    qDeleteAll(m_samples);
    m_samples.clear();
}

void TrackEngine::init(const QString &fileName)
{
    m_fileName = fileName;
    read();
}

QStringList TrackEngine::samples() const
{
    return m_samples.keys();
}

QString TrackEngine::filePath(const QString &sampleName) const
{
    Sample *sample = m_samples[sampleName];
    if (sample == nullptr)
    {
        return QString();
    }
    return sample->filePath;
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

qint64 TrackEngine::startTime(const QString &sampleName) const
{
    Sample *sample = m_samples[sampleName];
    if (sample == nullptr)
    {
        return -1;
    }
    return sample->startTime;
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
    // clear existing data
    qDeleteAll(m_samples);
    m_samples.clear();

    GOOGLE_PROTOBUF_VERIFY_VERSION;
    m_track.Clear();

    std::fstream protoInput(m_fileName.toStdString().c_str(), std::ios::in | std::ios::binary);

    if (!protoInput)
    {
        // TODO : error codes for string table lookup
        return; // failed to open input stream
    }

    bool isParsed = m_track.ParseFromIstream(&protoInput);
    protoInput.close();

    if (!isParsed)
    {
        // TODO : error codes for string table lookup
        return; // failed to parse proto file
    }

    for (auto &trackSample : m_track.samples())
    {
        Sample *sample = new Sample();
        sample->sampleName = "";
        sample->groupName = "";
        sample->filePath = QString::fromStdString(trackSample.filepath());
        sample->frequency = trackSample.frequency();
        sample->probability = trackSample.probability();
        sample->volumeMultiplier = trackSample.volumemultiplier();
        sample->startTime = trackSample.starttime();

        m_samples.insert(sample->sampleName, sample);
        m_groups.insert(sample->groupName, sample->sampleName);
    }
}

void TrackEngine::write()
{

}
