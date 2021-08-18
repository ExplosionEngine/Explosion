//
// Created by John Kindem on 2021/5/16 0016.
//

#ifndef EXPLOSION_BUFFER_H
#define EXPLOSION_BUFFER_H

#include <RHI/Enum.h>

namespace Explosion::RHI {
    class Buffer {
    public:
        struct Config {
            uint32_t size;
            BufferUsageFlags usages;
            MemoryPropertyFlags memoryProperties;
        };

        virtual ~Buffer();
        virtual uint32_t GetSize() = 0;
        virtual void UpdateData(void* data) = 0;

    protected:
        explicit Buffer(Config config);

        Config config;
    };
}

#endif //EXPLOSION_BUFFER_H
