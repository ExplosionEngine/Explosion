//
// Created by Mevol on 2021/8/8.
//

#ifndef EXPLOSION_STREAM_H
#define EXPLOSION_STREAM_H

#include <fstream>
#include <FileSystem/Common.h>
#include <FileSystem/File.h>

namespace Explosion::FileSystem {
    template <typename T>
    class Stream {
    public:

        explicit Stream(File& file)
            : openFlag(false), openType(FileType::TEXT), failFlag(true), file(file) {}

        virtual ~Stream() = default;

        Stream(Stream<T>&) = delete;

        void operator=(Stream<T>&) = delete;

        bool IsOpen();

        virtual void Open(const FileType& fileType);

        bool Fail();

        void Close();

        [[nodiscard]] size_t Size() const
        {
            return static_cast<T*>(this)->MakeImpl();
        }

    protected:
        bool openFlag;
        bool failFlag;
        FileType openType;
        std::fstream fileStream;
        File& file;
    };
}

#endif //EXPLOSION_STREAM_H
