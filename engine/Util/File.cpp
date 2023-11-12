#include "File.h"
#include <fstream>

std::vector<char> xUtil::xFile::Read(const std::string& fileName, const std::ios::openmode& mode)
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

std::vector<char> xUtil::xFile::ReadAsBin(const std::string &fileName)
{
    return Read(fileName, std::ios::binary);
}

std::vector<char> xUtil::xFile::ReadAsText(const std::string &fileName)
{
    return Read(fileName, std::ios::in);
}
