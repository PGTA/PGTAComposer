
#pragma once

#include <stddef.h>

class PGTATrackTreeModel;

namespace FlatbufferTrackLoader
{
    PGTATrackTreeModel* LoadTrack(const char* src, const size_t length, PGTATrackTreeModel* trackModel);
}
