
#include <QVariant>
#include <QByteArray>
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

static const size_t MAX_TRACK_LEN = (1 << 16);

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
        qDebug("%s", parser.error_.c_str());
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
        QVector<QVariant> group(TrackTreeModel::SampleColumn_Size);
        if (!schemaGroup)
        {
            continue;
        }

        const flatbuffers::String* groupName = schemaGroup->name();
        if (!groupName || groupName->size() == 0)
        {
            continue;
        }
        group[TrackTreeModel::GroupColumn_Name] = QString(groupName->c_str());

        const flatbuffers::String* groupUuid = schemaGroup->uuid();
        if (!groupUuid)
        {
            continue;
        }

        QUuid uuid = QString::fromStdString(AddUuidFormatting(groupUuid->c_str()));

        if (uuid.isNull())
        {
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
        QVector<QVariant> sample(TrackTreeModel::SampleColumn_Size);
        if (!schemaSample)
        {
            continue;
        }

        const flatbuffers::String* sampleName = schemaSample->name();
        if (!sampleName || sampleName->size() == 0)
        {
            continue;
        }
        sample[TrackTreeModel::SampleColumn_Name] = QString(sampleName->c_str());

        const flatbuffers::String* sampleDefaultFile = schemaSample->defaultFile();
        if (!sampleDefaultFile || sampleDefaultFile->size() == 0)
        {
            continue;
        }
        sample[TrackTreeModel::SampleColumn_DefaultFile] = QString(sampleDefaultFile->c_str());

        const qint64 sampleStartTime = schemaSample->startTime();
        if (sampleStartTime < 0)
        {
            continue;
        }
        sample[TrackTreeModel::SampleColumn_StartTime] = sampleStartTime;

        const qint64 sampleFrequency = schemaSample->frequency();
        if (sampleFrequency < 0)
        {
            continue;
        }
        sample[TrackTreeModel::SampleColumn_Frequency] = sampleFrequency;

        const float sampleProbability = schemaSample->probability();
        if (sampleProbability < 0.0f)
        {
            continue;
        }
        sample[TrackTreeModel::SampleColumn_Probability] = sampleProbability;

        const float sampleVolumeMultiplier = schemaSample->volumeMultiplier();
        if(sampleVolumeMultiplier < 0.0f)
        {
            continue;
        }
        sample[TrackTreeModel::SampleColumn_VolumeMultiplier] = schemaSample->volumeMultiplier();

        QUuid groupUuid;
        const flatbuffers::String* schemaUuid = schemaSample->group();
        if(schemaUuid)
        {
            groupUuid = QString::fromStdString(AddUuidFormatting(schemaUuid->c_str()));
            if (groupUuid.isNull())
            {
                continue;
            }
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

