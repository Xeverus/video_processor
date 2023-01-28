#pragma once

#include <string>
#include <vector>

namespace vid_lib::filesystem
{

class BinaryFile
{
public:
    static std::vector<char> ReadWhole(const std::string& filepath);
};

}
