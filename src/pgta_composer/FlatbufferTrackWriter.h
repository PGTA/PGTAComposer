
#pragma once

#include <stddef.h>

class TrackTreeModel;

namespace FlatBufferTrackWriter
{
    TrackTreeModel* WriteTrack(const TrackTreeModel* trackModel, const char* dest);
}
