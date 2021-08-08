//
// Created by John Kindem on 2021/8/8.
//

#ifndef EXPLOSION_STREAM_H
#define EXPLOSION_STREAM_H

#include <fstream>

#include <FileSystem/Common.h>

namespace Explosion::FileSystem {
    class File;

    template <typename T>
    class Stream {
    public:
        explicit Stream(File& file);
        virtual ~Stream();
        Stream(Stream<T>&) = delete;
        void operator=(Stream<T>&) = delete;

        bool IsOpen() const;
        virtual void Open(const FileType& fileType);
        bool Fail();
        void Close();
        size_t Size() const;

    protected:
        bool openFlag;
        bool failFlag;
        FileType openType;
        std::fstream fileStream;
        File& file;
    };
}

#endif //EXPLOSION_STREAM_H
