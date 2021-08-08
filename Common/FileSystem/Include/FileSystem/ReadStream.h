//
// Created by Mevol on 2021/8/8.
//

#ifndef EXPLOSION_READSTREAM_H
#define EXPLOSION_READSTREAM_H

#include <FileSystem/Stream.h>

namespace Explosion::FileSystem {
    class ReadStream : public Stream<ReadStream> {
    public:
        explicit ReadStream(File& file);
        ~ReadStream() override;
        ReadStream(ReadStream&) = delete;
        void operator=(ReadStream&) = delete;

        template <typename T>
        ReadStream& operator>>(T& t) {
            if(IsOpen()) {
                fileStream >> t;
                return *this;
            }
            throw std::runtime_error("ReadStream::operator>>(): Stream Open Failed!");
        }

        ReadStream& operator()(uint32_t pos);
        void Open(const FileType& fileType) override;

    private:
        friend Stream<ReadStream>;

        size_t SizeImpl();
    };
}

#endif //EXPLOSION_READSTREAM_H
