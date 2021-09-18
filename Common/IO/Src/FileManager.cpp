//
// Created by Mevol on 2021/8/8.
//

#include <fstream>
#include <Common/Exception.h>
#include <IO/FileManager.h>

namespace Explosion::IO {
    template <typename T>
    static void ReadInternal(const std::string& filename, bool binary, T& output, uint32_t stride = 1)
    {
        EXPLOSION_ASSERT(stride != 0, "Invalid read stride");
        auto mode = binary ? std::ios::ate | std::ios::binary : std::ios::ate;
        std::ifstream file(filename, mode);
        if (!file.is_open()) {
            throw std::runtime_error(std::string("failed to open file: ") + filename);
        }
        size_t fileSize = (size_t) file.tellg();
        output.resize(fileSize / stride);
        file.seekg(0);
        file.read(output.data(), static_cast<long>(fileSize));
        file.close();
    }

    std::vector<char> FileManager::ReadFile(const std::string& filename, bool binary)
    {
        std::vector<char> buffer;
        ReadInternal(filename, binary, buffer, 1);
        return buffer;
    }

    std::string FileManager::ReadFile(const std::string& filename)
    {
        std::string output;
        ReadInternal(filename, false, output, 1);
        return output;
    }

    void FileManager::WriteFile(const std::string& filename, const char* data, uint32_t size, bool binary)
    {
        auto mode = binary ? std::ios::ate | std::ios::binary : std::ios::ate;
        std::ofstream file(filename, mode);
        if (!file.is_open()) {
            throw std::runtime_error(std::string("failed to open file: ") + filename);
        }
        file.write(data, size);
    }
}
