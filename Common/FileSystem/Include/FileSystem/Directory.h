//
// Created by John Kindem on 2021/8/8.
//

#ifndef EXPLOSION_DIRECTORY_H
#define EXPLOSION_DIRECTORY_H

#include <FileSystem/Entry.h>

namespace Explosion::FileSystem {
    class File;

    class Directory : public Entry<Directory> {
    public:
        explicit Directory(const std::string& path);
        ~Directory() override;
        Directory(const Directory& directory);
        Directory& operator=(const Directory& directory);

    public:
        std::vector<File> ListFile();
        std::vector<Directory> ListDir();

    private:
        friend Entry<Directory>;

        void MakeImpl();
    };
}

#endif //EXPLOSION_DIRECTORY_H
