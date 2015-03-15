
#include <QVariant>
#include <QByteArray>
#include <QString>
#include <sstream>
#include <schema/track_generated.h>
#include <schema/track.fbs.h>
#include <flatbuffers/idl.h>
#include "FlatbufferTrackLoader.h"
#include "TrackTreeModel.h"

static TrackTreeModel* LoadBinaryTrack(const uint8_t* src, const size_t length, TrackTreeModel* trackModel);
static TrackTreeModel* LoadAsciiTrack(const char* src, TrackTreeModel* trackModel);
static TrackTreeModel* InitTrackData(TrackTreeModel* const trackModel, const PGTASchema::Track* trackSchema);
static std::string AddUuidFormatting(const std::string &uuid);
static std::string GetFileNameFromPath(const std::string &filePath);

static const size_t MAX_TRACK_LEN = (1 << 16);
const static int UUID_NUM_BYTES = 32;

TrackTreeModel* FlatbufferTrackLoader::LoadTrack(const char* src, const size_t length, TrackTreeModel* trackModel)
{
    if (!src || length > MAX_TRACK_LEN || !trackModel)
    {
        return nullptr;
    }

    if (PGTASchema::TrackBufferHasIdentifier(src))
    {
        return LoadBinaryTrack(reinterpret_cast<const uint8_t*>(src), length, trackModel);
    }
    else
    {
        return LoadAsciiTrack(src, trackModel);
    }
}

static TrackTreeModel* LoadBinaryTrack(const uint8_t* src, const size_t length, TrackTreeModel* trackModel)
{
    flatbuffers::Verifier verifier(src, length);
    if (!PGTASchema::VerifyTrackBuffer(verifier))
    {
        qDebug("Verification of track buffer failed.");
        return nullptr;
    }
    const PGTASchema::Track* trackSchema = PGTASchema::GetTrack(src);
    return InitTrackData(trackModel, trackSchema);
}

static TrackTreeModel* LoadAsciiTrack(const char* src, TrackTreeModel* trackModel)
{
    qDebug("Lodaing Ascii track");
    flatbuffers::Parser parser;
    if (!parser.Parse(PGTASchemaHeader::TRACK_FBS) || !parser.Parse(src))
    {
        qDebug("Lodaing Ascii track parse error.");
        qCritical("%s", parser.error_.c_str());
        return nullptr;
    }
    const PGTASchema::Track* trackSchema = PGTASchema::GetTrack(parser.builder_.GetBufferPointer());
    return InitTrackData(trackModel, trackSchema);
}

static TrackTreeModel* InitTrackData(TrackTreeModel* const trackModel, const PGTASchema::Track* trackSchema)
{
    using SampleList = flatbuffers::Vector<flatbuffers::Offset<PGTASchema::Sample>>;
    using GroupList = flatbuffers::Vector<flatbuffers::Offset<PGTASchema::Group>>;
    using RestrictionList = flatbuffers::Vector<flatbuffers::Offset<PGTASchema::Restriction>>;

    qDebug("Initializing Track");
    if (trackSchema == nullptr || trackSchema->samples() == nullptr)
    {
        return nullptr;
    }

    // Load Track Groups with Google Flatbuffer
    const GroupList* schemaGroups = trackSchema->groups();
    for (const PGTASchema::Group* schemaGroup : *schemaGroups)
    {
        if (!schemaGroup)
        {
            qWarning("Problem reading group.");
            continue;
        }
        QVector<QVariant> group(TrackTreeModel::SampleColumn_Size);

        const flatbuffers::String* groupName = schemaGroup->name();
        if (!groupName || groupName->size() == 0)
        {
           qWarning("Group name not specified.");
        }
        group[TrackTreeModel::GroupColumn_Name] = QString(groupName->c_str());

        const flatbuffers::String* groupUuid = schemaGroup->uuid();
        if (!groupUuid || groupUuid->Length() < UUID_NUM_BYTES)
        {
            qWarning("Group UUID not specified, skipping group.");
            continue;
        }

        QUuid uuid = QString::fromStdString(AddUuidFormatting(groupUuid->c_str()));
        if (uuid.isNull())
        {
            qWarning("Could not parse group UUID.");
            continue;
        }
        group[TrackTreeModel::GroupColumn_UUID] = uuid;
        trackModel->addGroup(group, uuid);
    }

    // Load Track Restrictions with Google Flatbuffer
    const RestrictionList* schemaRestrictions = trackSchema->restrictions();
    for(const PGTASchema::Restriction* schemaRestriction : *schemaRestrictions)
    {
        const flatbuffers::String* schemaGroup1Uuid = schemaRestriction->group1();
        if (!schemaGroup1Uuid)
        {
            continue;
        }

        QUuid group1Uuid = QString(schemaGroup1Uuid->c_str());
        if (group1Uuid.isNull())
        {
            continue;
        }

        // Add group 1 to model here

        const flatbuffers::String*  schemaGroup2Uuid = schemaRestriction->group2();
        if(!schemaGroup2Uuid)
        {
            continue;
        }

        QUuid group2Uuid = QString(schemaGroup2Uuid->c_str());
        if (group2Uuid.isNull())
        {
            continue;
        }
        // Add group 2 to model here
    }

    // Load Track Samples with Google Flatbuffer
    const SampleList* schemaSamples = trackSchema->samples();
    for (const PGTASchema::Sample* schemaSample : *schemaSamples)
    {
        if (!schemaSample)
        {
            qWarning("Problem reading sample.");
            continue;
        }
        QVector<QVariant> sample(TrackTreeModel::SampleColumn_Size);

        const flatbuffers::String* sampleDefaultFile = schemaSample->defaultFile();
        if (!sampleDefaultFile || sampleDefaultFile->size() == 0)
        {
            qWarning("Sample default file not specified.");
        }
        else
        {
            sample[TrackTreeModel::SampleColumn_DefaultFile] = QString(sampleDefaultFile->c_str());
        }

        std::string sampleName;
        const flatbuffers::String* schemaSampleName = schemaSample->name();
        if (!schemaSampleName || schemaSampleName->size() == 0)
        {
            qWarning("Sample name not specified, setting name based on default file path.");
            if (sampleDefaultFile->size() > 0)
            {
                sampleName = GetFileNameFromPath(sampleDefaultFile->c_str());
            }
        }
        else
        {
            sampleName = schemaSampleName->c_str();
        }
        sample[TrackTreeModel::SampleColumn_Name] = QString(sampleName.c_str());

        float sampleStartTime = schemaSample->startTime();
        if (sampleStartTime < 0.0f)
        {
            qWarning("Sample start time not valid (%f), setting to default value.", sampleStartTime);
            sampleStartTime = 0;
        }
        sample[TrackTreeModel::SampleColumn_StartTime] = sampleStartTime;

        float samplePeriod = schemaSample->period();
        if (samplePeriod < 0.0f)
        {
            qWarning("Sample period not valid (%f), setting to default value.", samplePeriod);
            samplePeriod = 0.0f;
        }
        sample[TrackTreeModel::SampleColumn_Period] = samplePeriod;

        float samplePeriodDeviation = schemaSample->periodDeviation();
        if (qAbs(samplePeriodDeviation) > samplePeriod/2)
        {
            qWarning("Sample period deviation not valid (%f), setting to default value.", samplePeriodDeviation);
            samplePeriodDeviation = (samplePeriodDeviation < 0) ? -1.0f * samplePeriod/2 : samplePeriod/2;
        }
        sample[TrackTreeModel::SampleColumn_PeriodDeviation] = samplePeriodDeviation;

        float sampleProbability = schemaSample->probability();
        if (sampleProbability < 0.0f || sampleProbability > 1.0f)
        {
            qWarning("Sample probability not valid (%f), setting to default value.", sampleProbability);
            sampleProbability = (sampleProbability > 1.0f) ? 1.0f : 0.0f;
        }
        sample[TrackTreeModel::SampleColumn_Probability] = sampleProbability;

        float sampleVolume = schemaSample->volume();
        if(sampleVolume < -95.0f || sampleVolume > 6.0f)
        {
            qWarning("Sample volume not valid (%f), setting to default value.", sampleVolume);
            sampleVolume = (sampleVolume > 6.0f) ? 6.0f : -95.0f;
        }
        sample[TrackTreeModel::SampleColumn_Volume] = sampleVolume;

        QUuid groupUuid;
        const flatbuffers::String* schemaUuid = schemaSample->group();
        if(schemaUuid && schemaUuid->Length() == UUID_NUM_BYTES)
        {
            groupUuid = QString::fromStdString(AddUuidFormatting(schemaUuid->c_str()));
            sample[TrackTreeModel::SampleColumn_GroupUUID] = groupUuid;
        }
        trackModel->addSample(sample, groupUuid);
    }
    return trackModel;
}

static std::string AddUuidFormatting(const std::string &uuid)
{
    if (uuid.length() != 32)
    {
        return "";
    }
    std::stringstream ss;
    ss << "{"
       << uuid.substr(0, 8)
       << "-"
       << uuid.substr(8, 4)
       << "-"
       << uuid.substr(12,4)
       << "-"
       << uuid.substr(16,4)
       << "-"
       << uuid.substr(20,12)
       << "}\n";
    return ss.str();
}

static std::string GetFileNameFromPath(const std::string &filePath)
{
   const std::size_t slashIndex = filePath.find_last_of("\\/");
   const std::size_t periodIndex = filePath.find_last_of(".");
   std::size_t length = 0;
   if (periodIndex > slashIndex)
   {
       length = periodIndex - slashIndex - 1;
   }
   return filePath.substr(slashIndex+1, length);
}

