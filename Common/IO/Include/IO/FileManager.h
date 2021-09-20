//
// Created by John Kindem on 2021/4/10.
//

#ifndef EXPLOSION_IO_FILE_H
#define EXPLOSION_IO_FILE_H

#include <vector>
#include <string>

namespace Explosion::IO {
    class FileManager {
    public:
        static std::vector<char> ReadFile(const std::string& filename, bool binary);
        static std::string ReadFile(const std::string& filename);
        static void WriteFile(const std::string& filename, const char* data, uint32_t size, bool binary);

        FileManager() = delete;
        ~FileManager() = delete;
    };
}

#endif //EXPLOSION_IO_FILE_H
