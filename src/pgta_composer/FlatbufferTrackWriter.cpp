

#include <QVariant>
#include <QByteArray>
#include <QUuid>
#include <string>
#include <limits>
#include <math.h>
#include <PGTA/schema/track_generated.h>
#include <PGTA/schema/track.fbs.h>
#include "flatbuffers/flatbuffers.h"
#include <flatbuffers/idl.h>
#include "FlatbufferTrackWriter.h"
#include "PGTATrackTreeModel.h"
#include "PGTATrackItem.h"
#include "FileUtils.h"

using SchemaSamples = std::vector<flatbuffers::Offset<PGTASchema::Sample>>;
using SchemaGroups = std::vector<flatbuffers::Offset<PGTASchema::Group>>;
using SchemaRestrictions = std::vector<flatbuffers::Offset<PGTASchema::Restriction>>;
using Builder = flatbuffers::FlatBufferBuilder;

static void AddTrackItem(const PGTATrackItem *root, Builder &fbb, SchemaSamples &samples, SchemaGroups &groups, bool ue4);
static std::string GetFileNameFromPath(const std::string &filePath);
static void RemoveUuidFormatting(std::string &uuid);
static float IntToGain(int intGain);

bool FlatBufferTrackWriter::WriteTrack(const PGTATrackTreeModel* trackModel, const std::string dest, bool ue4,
                                       bool binary)
{
    Builder fbb;
    SchemaSamples samples;
    SchemaGroups groups;
    SchemaRestrictions restrictions;

    const PGTATrackItem *root = trackModel->getRoot();
    AddTrackItem(root, fbb, samples, groups, ue4);

    auto fbSamples = fbb.CreateVector(samples);
    auto fbGroups = fbb.CreateVector(groups);
    auto fbRestrictions = fbb.CreateVector(restrictions);
    auto track = PGTASchema::CreateTrack(fbb,fbSamples, fbGroups, fbRestrictions, trackModel->getIsMeasuredInBeats());
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

static void AddTrackItem(const PGTATrackItem *root, Builder &fbb, SchemaSamples &samples, SchemaGroups &groups,
                         bool ue4)
{
    int numChildren = root->ChildCount();

    for (int i = 0; i < numChildren; ++i)
    {
        const PGTATrackItem *child = root->GetChild(i);
        if (child->IsGroup())
        {
            AddTrackItem(child, fbb, samples, groups, ue4);
            // Add Group (Gorup Builder)
            auto fbbName = fbb.CreateString(child->GetData(PGTATrackTreeModel::GroupColumn_Name).toString().toStdString());
            QUuid qUuid(child->GetData(PGTATrackTreeModel::GroupColumn_UUID).toString());
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
            flatbuffers::Offset<flatbuffers::String> fbbDefaultFile;

            if (ue4)
            {
                std::string defaultFile = "/Game/PGTATest/Audio/" +
                        GetFileNameFromPath(child->GetData(PGTATrackTreeModel::SampleColumn_DefaultFile).toString().toStdString());
                fbbDefaultFile = fbb.CreateString(defaultFile);
            }
            else
            {
                fbbDefaultFile = fbb.CreateString(
                            child->GetData(PGTATrackTreeModel::SampleColumn_DefaultFile).toString().toStdString());
            }


            auto fbbName = fbb.CreateString(child->GetData(PGTATrackTreeModel::SampleColumn_Name).toString().toStdString());
            QUuid qUuid(child->GetData(PGTATrackTreeModel::SampleColumn_GroupUUID).toString());
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
            sampleBuilder.add_startTime(child->GetData(PGTATrackTreeModel::SampleColumn_StartTime).toFloat());
            sampleBuilder.add_period(child->GetData(PGTATrackTreeModel::SampleColumn_Period).toFloat());
            sampleBuilder.add_periodDeviation(child->GetData(PGTATrackTreeModel::SampleColumn_PeriodDeviation).toFloat());
            sampleBuilder.add_probability(child->GetData(PGTATrackTreeModel::SampleColumn_Probability).toFloat());
            sampleBuilder.add_transitionWeight(child->GetData(PGTATrackTreeModel::SampleColumn_TransitionWeight).toFloat());
            sampleBuilder.add_gain(IntToGain(child->GetData(PGTATrackTreeModel::SampleColumn_Gain).toInt()));
            sampleBuilder.add_group(fbbUuid);
            auto sample = sampleBuilder.Finish();

            samples.push_back(sample);
        }
    }
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
   return filePath.substr(slashIndex + 1, length);
}

static void RemoveUuidFormatting(std::string &uuid)
{
    uuid.erase(std::remove(uuid.begin(), uuid.end(), '{'), uuid.end());
    uuid.erase(std::remove(uuid.begin(), uuid.end(), '-'), uuid.end());
    uuid.erase(std::remove(uuid.begin(), uuid.end(), '}'), uuid.end());
}

static float IntToGain(int intGain)
{
    double gain = 20.0f * log10(intGain/(std::numeric_limits<int>::max()/2.0f));
    gain = int(gain * 10)/10.0f;
    return gain;
}

