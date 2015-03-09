
#pragma once

#include <string>
#include <stddef.h>

class TrackTreeModel;

namespace FlatBufferTrackWriter
{
    bool WriteTrack(const TrackTreeModel* trackModel, const std::string dest, bool binary = false);
}
