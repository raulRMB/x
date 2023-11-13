#include "File.h"
#include <fstream>

namespace x
{
std::vector<char> File::Read(const std::string& fileName, const std::ios::openmode& mode)
{
    std::ifstream file(fileName, mode | std::ios::ate);

    if(!file.is_open())
    {
        throw std::runtime_error("Failed to open file: " + fileName);
    }

    std::streamsize fileSize = (std::streamsize)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    return buffer;
}

std::vector<char> File::ReadAsBin(const std::string &fileName)
{
    return Read(fileName, std::ios::binary);
}

std::vector<char> File::ReadAsText(const std::string &fileName)
{
    return Read(fileName, std::ios::in);
}
}