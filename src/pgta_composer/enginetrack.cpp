
#include <iostream>
#include <fstream>
#include "enginetrack.h"
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

EngineTrack::EngineTrack(QObject *parent) :
    QObject(parent)
{
}

EngineTrack::~EngineTrack()
{
    qDeleteAll(m_samples);
    m_samples.clear();
}

void EngineTrack::init(const QString &fileName)
{
    m_fileName = fileName;
    read();
}

QStringList EngineTrack::samples() const
{
    return m_samples.keys();
}

QString EngineTrack::filePath(const QString &sampleName) const
{
    Sample *sample = m_samples[sampleName];
    if (sample == nullptr)
    {
        return QString();
    }
    return sample->filePath;
}

QString EngineTrack::group(const QString &sampleName) const
{
    Sample *sample = m_samples[sampleName];
    if (sample == nullptr)
    {
        return QString();
    }
    return sample->groupName;
}

qint64 EngineTrack::frequency(const QString &sampleName) const
{
    Sample *sample = m_samples[sampleName];
    if (sample == nullptr)
    {
        return -1;
    }
    return sample->frequency;
}

qint32 EngineTrack::probability(const QString &sampleName) const
{
    Sample *sample = m_samples[sampleName];
    if (sample == nullptr)
    {
        return -1;
    }
    return sample->probability;
}


qreal EngineTrack::volumeMultiplier(const QString &sampleName) const
{
    Sample *sample = m_samples[sampleName];
    if (sample == nullptr)
    {
        return -1;
    }
    return sample->volumeMultiplier;
}

qint64 EngineTrack::startTime(const QString &sampleName) const
{
    Sample *sample = m_samples[sampleName];
    if (sample == nullptr)
    {
        return -1;
    }
    return sample->startTime;
}

QStringList EngineTrack::groups() const
{
   return m_groups.uniqueKeys();
}

QStringList EngineTrack::samplesByGroup(const QString &sampleName) const
{
    return m_groups.values(sampleName);
}

void EngineTrack::read()
{
    // clear existing data
    qDeleteAll(m_samples);
    m_samples.clear();

    GOOGLE_PROTOBUF_VERIFY_VERSION;
    PGTA::Track track;
    
    std::fstream protoInput(m_fileName.toStdString().c_str(), std::ios::in | std::ios::binary);

    if (!protoInput)
    {
        // TODO : error codes for string table lookup
        return; // failed to open input stream
    }

    bool isParsed = track.ParseFromIstream(&protoInput);
    protoInput.close();

    if (!isParsed)
    {
        // TODO : error codes for string table lookup
        return; // failed to parse proto file
    }

    for (int i = 0; i < track.samples_size(); ++i)
    {
        const PGTA::Track_Sample &trackSample = track.samples(i);
        Sample *sample = new Sample();
        sample->sampleName = QString::fromStdString("Sample " + std::to_string(i));
        sample->groupName = "Group 1";
        std::cout << sample->sampleName.toStdString() << std::endl;
        sample->filePath = QString::fromStdString(trackSample.filepath());
        sample->frequency = trackSample.frequency();
        sample->probability = trackSample.probability();
        sample->volumeMultiplier = trackSample.volumemultiplier();
        sample->startTime = trackSample.starttime();

        m_samples.insert(sample->sampleName, sample);
        m_groups.insert(sample->groupName, sample->sampleName);
    }
}

void EngineTrack::write()
{

}
