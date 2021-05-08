//
// Created by John Kindem on 2021/4/10.
//

#include <fstream>

#include <Explosion/Common/FileReader.h>

namespace Explosion {
    std::vector<char> FileReader::Read(const std::string& filename, bool binary)
    {
        auto mode = binary ? std::ios::ate | std::ios::binary : std::ios::ate;
        std::ifstream file(filename, mode);
        if (!file.is_open()) {
            throw std::runtime_error(std::string("failed to open file: ") + filename);
        }

        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), static_cast<long>(fileSize));
        file.close();
        return buffer;
    }

    FileReader::~FileReader() = default;

    FileReader::FileReader() = default;
}
