//
// Created by Mevol on 2021/8/8.
//

#include <FileSystem/Stream.h>
#include <FileSystem/File.h>

namespace Explosion::FileSystem {
    template <typename T>
    Stream<T>::Stream(File& file)
        : openFlag(false), openType(FileType::TEXT), failFlag(true), file(file) {}

    template <typename T>
    Stream<T>::~Stream() = default;

    template <typename T>
    bool Stream<T>::IsOpen() const
    {
        return openFlag;
    }

    template <typename T>
    void Stream<T>::Open(const FileType& fileType)
    {
        if(fileType == FileType::TEXT)
        {
            openType = FileType::TEXT;
            fileStream.open(file.GetAbsolutePath().c_str(),std::ios::in | std::ios::out);
            if(!fileStream.fail())
            {
                failFlag  = false;
                openFlag = true;
            }
        }
        if(fileType == FileType::BINARY)
        {
            openType == FileType::BINARY;
            fileStream.open(file.GetAbsolutePath().c_str(),std::ios::in | std::ios::out | std::ios::binary);
            if(!fileStream.fail())
            {
                failFlag  = false;
                openFlag = true;
            }
        }
    }

    template <typename T>
    bool Stream<T>::Fail()
    {
        return failFlag;
    }

    template <typename T>
    void Stream<T>::Close()
    {
        if (!IsOpen()) {
            return;
        }
        fileStream.close();
    }

    template <typename T>
    size_t Stream<T>::Size() const
    {
        return static_cast<T*>(this)->MakeImpl();
    }
}
