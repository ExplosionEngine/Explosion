//
// Created by Administrator on 2021-07-07.
//

#ifndef EXFILESYSTEM_FILESYSTEM_HPP
#define EXFILESYSTEM_FILESYSTEM_HPP

#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

#include <fstream>
#include <array>
#include <utility>
#include <vector>
#include <memory>
#include <string>
#include <stdexcept>
#include <sstream>
#include <cerrno>


namespace FileSystem {

    template <typename T>
    class Entry {
      public:

        explicit Entry(std::string path)
            : path(std::move(path)) {
        }

        explicit Entry(const Entry<T> &entry)
            : path(entry.path) {
        }


        const T& cast() const {
            return static_cast<const T&>(*this);
        }

        bool IsExists() const {
            return exists(path);
        }

        std::string GetAbsolutePath() const {
            if(IsExists())
                return absolute(path).string();
            throw std::runtime_error("Path not exist!");
        }

        std::string GetRelativePath(std::string inputPath) const {
            if(IsExists())
                return path.relative_path().string();
            throw std::runtime_error("Path not exist!");
        }

        Entry<T> GetParent() const {
            if(IsExists())
                return Entry<T>(path.parent_path().string);
            throw std::runtime_error("Path not exist!");
        }

        bool IsDirectory() const {
            if(IsExists())
                return fs::is_directory(path);
            return false;
        }

        bool IsFile() const {
            if(IsExists())
                return fs::is_regular_file(path);
            return false;
        }

        void Rename(const std::string& fullName) const {
            if(IsExists())
                rename(GetAbsolutePath().c_str(),fullName.c_str());
            throw std::runtime_error("Path not exist!");
        }

        Entry<T> &operator=(const Entry<T> &entry) {
            path = entry.path;
            return *this;
        }

        void Make() {
            static_cast<T*>(this)->MakeImpl();
        }

      protected:
        friend T;
        fs::path path;
    };

    enum class FileType {
        TEXT,
        BINARY
    };


    class File;
    class Directory : public Entry<Directory> {
    public:
#ifdef _WIN32
        static Directory DriverRoot(const std::string& name);

        // ...
#endif

        static Directory WorkDirectory();

        explicit Directory(const std::string &path) : Entry(path) {}

        std::vector<Entry<File>> ListFile() {
            std::vector<Entry<File>> result = {};
            if (!IsExists())
            {
                return result;
            }
            const fs::directory_entry fsentry(path);
            if (fsentry.status().type() != fs::file_type::directory)
            {
                return result;
            }
            fs::directory_iterator list(fsentry);
            for (auto& iter : list)
            {
                if(iter.status().type() != fs::file_type::directory)
                {
                    Entry<File> temp(iter.path().string());
                    result.push_back(temp);
                }
            }
            return result;
        }
        std::vector<Entry<Directory>> ListDir() {
            std::vector<Entry<Directory>> result = {};
            if (!IsExists())
            {
                return result;
            }
            const fs::directory_entry fsentry(path);
            if (fsentry.status().type() != fs::file_type::directory)
            {
                return result;
            }
            fs::directory_iterator list(fsentry);
            for (auto& iter : list)
            {
                if(iter.status().type() == fs::file_type::directory)
                {
                    Entry<Directory> temp(iter.path().string());
                    result.push_back(temp);
                }
            }
            return result;
        }

    private:
        friend Entry<Directory>;

        void MakeImpl();
    };

    class File;
    template <typename T>
    class Stream {
    public:

        explicit Stream(File* ifile) : openFlag(false),failFlag(true),file(ifile) {}

        bool IsOpen() {
            return openFlag;
        }

        virtual void Open(const FileType& fileType) {
            if(fileType == FileType::TEXT)
            {
                openType = FileType::TEXT;
                fileStream.open(file->GetAbsolutePath().c_str(),std::ios::in | std::ios::out);
                if(!fileStream.fail())
                {
                    failFlag  = false;
                    openFlag = true;
                }
            }
            if(fileType == FileType::BINARY)
            {
                openType == FileType::BINARY;
                fileStream.open(file->GetAbsolutePath().c_str(),std::ios::in | std::ios::out | std::ios::binary);
                if(!fileStream.fail())
                {
                    failFlag  = false;
                    openFlag = true;
                }
            }
            return;
        }

        bool Fail() {
            return failFlag;
        }

        void Close(){
            if(IsOpen())
            {
                fileStream.close();
            }
            return;
        }

        uint32_t Size(){
            return static_cast<T*>(this)->MakeImpl();
        }

    protected:
        bool openFlag;
        bool failFlag;
        FileType openType;
        std::fstream fileStream;
        const File *file;
    };


    class ReadStream : public Stream<ReadStream> {
    public:
        ReadStream(File* file) : Stream<ReadStream>(file){}
        template <typename T>
        ReadStream& operator>>(T &t) {
            throw std::runtime_error("ReadStream::operator>>(): unrealized type!");
        }
        ReadStream& operator>>(std::string &str) {
            if(IsOpen())
            {
                fileStream >> str;
                return *this;
            }
            throw std::runtime_error("ReadStream::operator>>(): Stream Open Failed!");
        }
        ReadStream& operator>>(int &num) {
            if(IsOpen())
            {
                fileStream >> num;
                return *this;
            }
            throw std::runtime_error("ReadStream::operator>>(): Stream Open Failed!");
        }
        ReadStream& operator()(uint32_t pos) {
            if(IsOpen())
            {
                fileStream.seekg(pos);
                return *this;
            }
            throw std::runtime_error("ReadStream::operator()(): Stream Open Failed!");
        }
        void Open(const FileType &fileType) {
            if (fileType == FileType::TEXT)
            {
                openType = FileType::TEXT;
                fileStream.open(file->GetAbsolutePath().c_str(),std::ios::in );
                if(!fileStream.fail())
                {
                    failFlag  = false;
                    openFlag = true;
                }
            }
            if(fileType == FileType::BINARY)
            {
                openType == FileType::BINARY;
                fileStream.open(file->GetAbsolutePath().c_str(),std::ios::in | std::ios::binary);
                if(!fileStream.fail())
                {
                    failFlag  = false;
                    openFlag = true;
                }
            }
            return;
        }
    private:
        friend Stream<ReadStream>;
        uint32_t SizeImpl() {
            if(IsOpen())
            {
                return fileStream.tellg();
            }
            return 0;
        }
    };



    class WriteStream : public Stream<WriteStream> {
    public:
        explicit WriteStream(File* file) : Stream(file){}
        template <typename T>
        WriteStream& operator<<(const T &t) {
            throw std::runtime_error("WriteStream::operator<<(): unrealized type!");
        }
        WriteStream& operator<<(const std::string &str) {
            if(IsOpen())
            {
                fileStream << str;
                return *this;
            }
            throw std::runtime_error("ReadStream::operator<<(): Stream Open Failed!");
        }
        WriteStream& operator<<(const int &num) {
            if(IsOpen())
            {
                fileStream << num;
                return *this;
            }
            throw std::runtime_error("ReadStream::operator<<(): Stream Open Failed!");
        }
        WriteStream& operator()(uint32_t pos) {
            if(IsOpen())
            {
                fileStream.seekp(pos);
                return *this;
            }
            throw std::runtime_error("ReadStream::operator()(): Stream Open Failed!");
        }
        void Open(const FileType &fileType)
        {
            if(fileType == FileType::TEXT) {
                openType = FileType::TEXT;
                fileStream.open(file->GetAbsolutePath().c_str(), std::ios::out);
                if(!fileStream.fail()){
                    failFlag  = false;
                    openFlag = true;
                }
            }
            if(fileType == FileType::BINARY)
            {
                openType == FileType::BINARY;
                fileStream.open(file->GetAbsolutePath().c_str(), std::ios::out | std::ios::binary);
                if(!fileStream.fail())
                {
                    failFlag  = false;
                    openFlag = true;
                }
            }
            return;
        }
    private:
        friend Stream<WriteStream>;
        uint32_t SizeImpl(){
            if(IsOpen())
            {
                return fileStream.tellp();
            }
            return 0;
        }
    };


    class File : public Entry<File> {
      public:
        File(const std::string &path) : Entry(path) {}
        File(const File &file) : Entry(file.path.string()) {
            path = file.path;
        }

        std::string GetName() {
            if(IsFile())
                return path.stem().string();
            throw std::runtime_error("Path not exist!");
        }

        std::string GetFullName() {
            if(IsFile())
                return path.filename().string();
            throw std::runtime_error("Path not exist!");
        }

        std::string GetExtension() {
            if(IsFile())
                return path.extension().string();
            throw std::runtime_error("Path not exist!");
        }

        template<typename T>
        Stream<T> OpenStream(const FileType& fileType) {
            File curFile(*this);
            Stream<T> retStream(&curFile);
            retStream.Open(fileType);
            return retStream;
        }

        ReadStream OpenReadStream(const FileType& fileType) {
            File curFile(*this);
            ReadStream retStream(&curFile);
            retStream.Open(fileType);
            return retStream;
        }

        WriteStream OpenWriteStream(const FileType& fileType) {
            File curFile(*this);
            WriteStream retStream(&curFile);
            retStream.Open(fileType);
            return retStream;
        }

        File &operator=(const File &file) {
            path = file.path;
            return *this;
        }

      private:
        friend Entry<File>;

        void MakeImpl(){
            if(IsExists())
                return ;
            WriteStream wstream = OpenWriteStream(FileType::BINARY);
            wstream.Open(FileType::BINARY);
            wstream.Close();
        }
    };
}


#endif //EXFILESYSTEM_FILESYSTEM_HPP
