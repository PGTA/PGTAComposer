

#include <QVariant>
#include <QByteArray>
#include <QUuid>
#include <string>
#include <schema/track_generated.h>
#include <schema/track.fbs.h>
#include "flatbuffers/flatbuffers.h"
#include <flatbuffers/idl.h>
#include "FlatbufferTrackWriter.h"
#include "TrackTreeModel.h"
#include "TrackItem.h"
#include "FileUtils.h"

using SchemaSamples = std::vector<flatbuffers::Offset<PGTASchema::Sample>>;
using SchemaGroups = std::vector<flatbuffers::Offset<PGTASchema::Group>>;
using SchemaRestrictions = std::vector<flatbuffers::Offset<PGTASchema::Restriction>>;
using Builder = flatbuffers::FlatBufferBuilder;

static void AddTrackItem(const TrackItem *root, Builder &fbb, SchemaSamples &samples, SchemaGroups &groups);
static void RemoveUuidFormatting(std::string &uuid);

bool FlatBufferTrackWriter::WriteTrack(const TrackTreeModel* trackModel, const std::string dest, bool binary)
{
    Builder fbb;
    SchemaSamples samples;
    SchemaGroups groups;
    SchemaRestrictions restrictions;

    const TrackItem *root = trackModel->getRoot();
    AddTrackItem(root, fbb, samples, groups);

    auto fbSamples = fbb.CreateVector(samples);
    auto fbGroups = fbb.CreateVector(groups);
    auto fbRestrictions = fbb.CreateVector(restrictions);
    auto track = PGTASchema::CreateTrack(fbb,fbSamples, fbGroups, fbRestrictions);
    fbb.Finish(track);

    if (binary)
    {
        return FileUtils::WriteBinaryToFile(dest, fbb.GetBufferPointer(), fbb.GetSize());
    }

    std::string jsongen;
    flatbuffers::Parser parser;
    if (!parser.Parse(PGTASchemaHeader::TRACK_FBS))
    {
        return false;
    }
    flatbuffers::GenerateText(parser, fbb.GetBufferPointer(), flatbuffers::GeneratorOptions(), &jsongen);
    return FileUtils::WriteAsciiToFile(dest, jsongen);
}

static void AddTrackItem(const TrackItem *root, Builder &fbb, SchemaSamples &samples, SchemaGroups &groups)
{
    int numChildren = root->ChildCount();

    for (int i = 0; i < numChildren; ++i)
    {
        const TrackItem *child = root->GetChild(i);
        if (child->IsGroup())
        {
            AddTrackItem(child, fbb, samples, groups);
            // Add Group (Gorup Builder)
            auto fbbName = fbb.CreateString(child->GetData(TrackTreeModel::GroupColumn_Name).toString().toStdString());
            QUuid qUuid(child->GetData(TrackTreeModel::GroupColumn_UUID).toString());
            std::string  uuid;
            if (!qUuid.isNull())
            {
                uuid = qUuid.toString().toStdString();
                RemoveUuidFormatting(uuid);
            }
            auto fbbUuid = fbb.CreateString(uuid);

            PGTASchema::GroupBuilder groupBuilder(fbb);
            // cannot call fbb.CreateString or fbb.CreateBuffer in this block
            groupBuilder.add_name(fbbName);
            groupBuilder.add_uuid(fbbUuid);
            auto group = groupBuilder.Finish();

            groups.push_back(group);
        }
        else
        {
            auto fbbDefaultFile = fbb.CreateString(
                        child->GetData(TrackTreeModel::SampleColumn_DefaultFile).toString().toStdString());
            auto fbbName = fbb.CreateString(child->GetData(TrackTreeModel::SampleColumn_Name).toString().toStdString());
            QUuid qUuid(child->GetData(TrackTreeModel::SampleColumn_GroupUUID).toString());
            std::string  uuid;
            if (!qUuid.isNull())
            {
                uuid = qUuid.toString().toStdString();
                RemoveUuidFormatting(uuid);
            }
            auto fbbUuid = fbb.CreateString(uuid);

            PGTASchema::SampleBuilder sampleBuilder(fbb);
            // cannot call fbb.CreateString or fbb.CreateBuffer in this block
            sampleBuilder.add_defaultFile(fbbDefaultFile);
            sampleBuilder.add_name(fbbName);
            sampleBuilder.add_startTime(child->GetData(TrackTreeModel::SampleColumn_StartTime).toFloat());
            sampleBuilder.add_period(child->GetData(TrackTreeModel::SampleColumn_Period).toFloat());
            sampleBuilder.add_periodDeviation(child->GetData(TrackTreeModel::SampleColumn_PeriodDeviation).toFloat());
            sampleBuilder.add_probability(child->GetData(TrackTreeModel::SampleColumn_Probability).toFloat());
            sampleBuilder.add_volume(child->GetData(TrackTreeModel::SampleColumn_Volume).toFloat());
            sampleBuilder.add_group(fbbUuid);
            auto sample = sampleBuilder.Finish();

            samples.push_back(sample);
        }
    }
}

static void RemoveUuidFormatting(std::string &uuid)
{
    uuid.erase(std::remove(uuid.begin(), uuid.end(), '{'), uuid.end());
    uuid.erase(std::remove(uuid.begin(), uuid.end(), '-'), uuid.end());
    uuid.erase(std::remove(uuid.begin(), uuid.end(), '}'), uuid.end());
}

