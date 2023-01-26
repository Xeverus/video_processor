#ifndef PROVID_FILE_FILE_H
#define PROVID_FILE_FILE_H

#include <string>
#include <vector>

namespace provid::file
{

class File
{
public:
    static std::vector<char> ReadWholeBinaryFile(const std::string& filepath);
};

}

#endif
