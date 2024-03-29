#ifndef X_FILE_H
#define X_FILE_H

#include <iostream>
#include <vector>
#include "../Core/defines.h"

namespace x
{
class File
{
public:
    static std::vector<char> Read(const std::string& fileName, const std::ios::openmode& mode);
    static std::vector<char> ReadAsBin(const std::string& fileName);
    static std::vector<char> ReadAsText(const std::string& fileName);
};
}

#endif //X_FILE_H
