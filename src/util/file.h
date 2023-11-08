//
// Created by Raul Romero on 2023-09-24.
//

#ifndef X_FILE_H
#define X_FILE_H

#include <iostream>
#include <vector>
#include "../base/defines.h"

namespace xUtil
{
    class xFile
    {
    public:
        static std::vector<char> Read(const std::string& fileName, const std::ios::openmode& mode);
        static std::vector<char> ReadAsBin(const std::string& fileName);
        static std::vector<char> ReadAsText(const std::string& fileName);
    };
}

#endif //X_FILE_H
