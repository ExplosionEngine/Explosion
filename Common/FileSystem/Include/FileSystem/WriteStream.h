//
// Created by Mevol on 2021/8/8.
//

#ifndef EXPLOSION_WRITESTREAM_H
#define EXPLOSION_WRITESTREAM_H

#include <FileSystem/Stream.h>

namespace Explosion::FileSystem {
    class WriteStream : public Stream<WriteStream> {
    public:
        explicit WriteStream(File& file);
        ~WriteStream() override;
        WriteStream(WriteStream&) = delete;
        void operator=(WriteStream&) = delete;

        template <typename T>
        WriteStream& operator<<(T&& value)
        {
            if (IsOpen()) {
                fileStream << value;
                return *this;
            }
            throw std::runtime_error("ReadStream::operator<<(): Stream Open Failed!");
        }

        WriteStream& operator()(uint32_t pos);
        void Open(const FileType& fileType) override;

    private:
        friend Stream<WriteStream>;

        uint32_t SizeImpl();
    };
}

#endif //EXPLOSION_WRITESTREAM_H
