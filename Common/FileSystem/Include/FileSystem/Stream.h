//
// Created by Mevol on 2021/8/8.
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

        explicit Stream(File& file)
            : openFlag(false), openType(FileType::TEXT), failFlag(true), file(file) {}

        virtual ~Stream() = default;

        Stream(Stream<T>&) = delete;

        void operator=(Stream<T>&) = delete;

        [[nodiscard]] bool IsOpen() const
        {
            return openFlag;
        }

        virtual void Open(const FileType& fileType)
        {
            if(fileType == FileType::TEXT) {
                openType = FileType::TEXT;
                fileStream.open(file.GetAbsolutePath().c_str(),std::ios::in | std::ios::out);
                if (!fileStream.fail()) {
                    failFlag  = false;
                    openFlag = true;
                }
            }
            if(fileType == FileType::BINARY) {
                openType = FileType::BINARY;
                fileStream.open(file.GetAbsolutePath().c_str(),std::ios::in | std::ios::out | std::ios::binary);
                if (!fileStream.fail()) {
                    failFlag  = false;
                    openFlag = true;
                }
            }
        }

        bool Fail()
        {
            return failFlag;
        }

        void Close()
        {
            if (!IsOpen()) {
                return;
            }
            fileStream.close();
        }

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
