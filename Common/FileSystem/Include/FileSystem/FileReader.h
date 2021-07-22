//
// Created by John Kindem on 2021/4/10.
//

#ifndef EXPLOSION_FILEREADER_H
#define EXPLOSION_FILEREADER_H

#include <vector>
#include <string>

namespace Explosion::FileSystem {
    class FileReader {
    public:
        static std::vector<char> Read(const std::string& filename, bool binary);

        static std::string Read(const std::string& filename);

        static void Write(const std::string& filename, const char* data, uint32_t size, bool binary);

        ~FileReader();

    private:
        FileReader();
    };
}

#endif //EXPLOSION_FILEREADER_H
