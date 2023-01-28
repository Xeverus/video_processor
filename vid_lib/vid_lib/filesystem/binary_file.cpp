#include "binary_file.h"

#include <fstream>

namespace vid_lib::filesystem
{

std::vector<char> BinaryFile::ReadWhole(const std::string& filepath)
{
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file: '" + filepath + "'");
    }

    const auto size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    file.read(buffer.data(), size);
    if (file.fail())
    {
        throw std::runtime_error("Failed to read whole file: '" + filepath + "'");
    }

    return buffer;
}

}