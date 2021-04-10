//
// Created by John Kindem on 2021/4/10.
//

#ifndef EXPLOSION_FILEREADER_H
#define EXPLOSION_FILEREADER_H

#include <vector>
#include <string>

namespace Explosion {
    class FileReader {
    public:
        static std::vector<char> Read(const std::string& filename);
        ~FileReader();

    private:
        FileReader();
    };
}

#endif //EXPLOSION_FILEREADER_H
