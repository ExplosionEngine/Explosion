//
// Created by johnk on 2022/1/23.
//

#ifndef EXPLOSION_RHI_BUFFER_H
#define EXPLOSION_RHI_BUFFER_H

#include <Common/Utility.h>
#include <RHI/Enum.h>

namespace RHI {
    struct BufferCreateInfo {
        size_t size;
        BufferUsageFlags usages;
    };

    class Buffer {
    public:
        NON_COPYABLE(Buffer)
        virtual ~Buffer();

        virtual void* Map(MapMode mapMode, size_t offset, size_t length) = 0;
        virtual void UnMap() = 0;
        virtual void Destroy() = 0;

    protected:
        explicit Buffer(const BufferCreateInfo* createInfo);
    };
}

#endif //EXPLOSION_RHI_BUFFER_H
