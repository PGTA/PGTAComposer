
#pragma once

#include <string>
#include <stddef.h>

class PGTATrackTreeModel;

namespace FlatBufferTrackWriter
{
    bool WriteTrack(const PGTATrackTreeModel* trackModel, const std::string dest, bool binary = false);
}
