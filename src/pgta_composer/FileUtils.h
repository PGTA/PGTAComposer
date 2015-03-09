
#pragma once

#include <string>
#include <stddef.h>

namespace FileUtils
{
     bool ReadBinaryFileToString(const std::string &filename, std::string &out);
     bool WriteBinaryToFile(const std::string &filename, const uint8_t *buffer, off_t size);
     bool WriteAsciiToFile(const std::string &filename, const std::string &buffer);
}
