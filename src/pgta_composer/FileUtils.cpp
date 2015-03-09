
#include "FileUtils.h"

#include <stdio.h>
#include <sys/stat.h>
#include <string>
#include <fstream>

namespace FileUtils
{
    off_t GetFileSize(int fd)
    {
        struct stat buf;
        auto ret = fstat(fd, &buf);
        return ret == 0 ? buf.st_size : -1;
    }

    off_t GetFileSize(FILE *f)
    {
        return GetFileSize(fileno(f));
    }

    bool ReadBinaryFileToString(const char *filename, std::string &out)
    {
        // try to open the file
        auto file = fopen(filename, "rb");
        if (file)
        {
            // get the size to read
            auto size = GetFileSize(file);

            // allocate enough space for size+null
            out.resize(size+1);

            // set the null char
            out[size] = '\0';

            // read the file to memory
            fread(&out[0], 1, size, file);

            // close the file
            fclose(file);
            return true;
        }
        return false;
    }

    bool ReadBinaryFileToString(const std::string &filename, std::string &out)
    {
        return ReadBinaryFileToString(filename.c_str(), out);
    }

    bool WriteBinaryToFile(const std::string &filename, const uint8_t *buffer, off_t size)
    {
        std::ofstream outFile(filename, std::ofstream::binary);
        if (!outFile)
        {
            return false;
        }
        outFile.write(reinterpret_cast<const char*>(buffer), size);
        outFile.close();
        return true;
    }

    bool WriteAsciiToFile(const std::string &filename, const std::string &buffer)
    {
        std::ofstream outFile(filename);
        if (!outFile)
        {
            return false;
        }
        outFile << buffer;
        outFile.close();
        return true;
    }
}
