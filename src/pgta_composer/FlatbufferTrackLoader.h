
#pragma once

#include <stddef.h>

class TrackTreeModel;

namespace FlatbufferTrackLoader
{
    TrackTreeModel* LoadTrack(const char* src, const size_t length, TrackTreeModel* trackModel);
}
