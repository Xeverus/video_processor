#pragma once

#include <string>
#include <vector>

namespace provid::filesystem
{

class BinaryFile
{
public:
    static std::vector<char> ReadWhole(const std::string& filepath);
};

}
