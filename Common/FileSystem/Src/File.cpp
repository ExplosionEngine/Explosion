//
// Created by John Kindem on 2021/8/8.
//

#include <FileSystem/File.h>
#include <FileSystem/WriteStream.h>

namespace Explosion::FileSystem {
    File::File(const std::string& path) : Entry<File>(path) {}

    File::~File() = default;

    File::File(const File& file) : Entry<File>(file.path.string()) {}

    File& File::operator=(const File& file)
    {
        path = file.path;
        return *this;
    }

    std::string File::GetName() const
    {
        if (IsFile()) {
            return path.stem().string();
        }
        throw std::runtime_error("Path not exist!");
    }

    std::string File::GetFullName() const
    {
        if (IsFile()) {
            return path.filename().string();
        }
        throw std::runtime_error("Path not exist!");
    }

    std::string File::GetExtension() const
    {
        if (IsFile()) {
            return path.extension().string();
        }
        throw std::runtime_error("Path not exist!");
    }

    void File::MakeImpl()
    {
        if (IsExists()) {
            return;
        }
        WriteStream stream(*this);
        stream.Open(FileType::TEXT);
        stream.Close();
    }
}
