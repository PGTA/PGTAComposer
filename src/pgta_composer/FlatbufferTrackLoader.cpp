
#include <QVariant>
#include <schema/track_generated.h>
#include <schema/track.fbs.h>
#include <flatbuffers/idl.h>
#include "FlatbufferTrackLoader.h"
#include "TrackTreeModel.h"

static TrackTreeModel* LoadBinaryTrack(const uint8_t* src, const size_t length, TrackTreeModel* trackModel);
static TrackTreeModel* LoadAsciiTrack(const char* src, const size_t length, TrackTreeModel* trackModel);
static TrackTreeModel* InitTrackData(TrackTreeModel* const trackModel, const PGTASchema::Track* trackSchema);

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
        return LoadAsciiTrack(src, length, trackModel);
    }
}

static TrackTreeModel* LoadBinaryTrack(const uint8_t* src, const size_t length, TrackTreeModel* trackModel)
{
    flatbuffers::Verifier verifier(src, length);
    if (!PGTASchema::VerifyTrackBuffer(verifier))
    {
        return nullptr;
    }

    const PGTASchema::Track* trackSchema = PGTASchema::GetTrack(src);

    return InitTrackData(trackModel, trackSchema);
}

static TrackTreeModel* LoadAsciiTrack(const char* src, const size_t length, TrackTreeModel* trackModel)
{
    flatbuffers::Parser parser;
    if (!parser.Parse(PGTASchemaHeader::TRACK_FBS) || !parser.Parse(src))
    {
        return nullptr;
    }

    const PGTASchema::Track* trackSchema = PGTASchema::GetTrack(parser.builder_.GetBufferPointer());

    return InitTrackData(trackModel, trackSchema);
}

static TrackTreeModel* InitTrackData(TrackTreeModel* const trackModel, const PGTASchema::Track* trackSchema)
{
    using SampleList = flatbuffers::Vector<flatbuffers::Offset<PGTASchema::Sample>>;
    using SchemaSample = PGTASchema::Sample;

    if (trackSchema == nullptr || trackSchema->samples() == nullptr)
    {
        return nullptr;
    }

    const SampleList* samples = trackSchema->samples();
    flatbuffers::uoffset_t numSamples = samples->size();

    for (flatbuffers::uoffset_t i = 0; i < numSamples; ++i)
    {
        QVector<QVariant> sample(TrackTreeModel::SampleColumn_Size);
        const SchemaSample* schemaSample = samples->Get(i);
        if(!schemaSample)
        {
            continue;
        }
        trackModel->addSample(sample, QUuid(0));
    }



    return trackModel;
}

