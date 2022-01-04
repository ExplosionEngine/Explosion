//
// Created by johnk on 3/1/2022.
//

#ifndef EXPLOSION_RHI_BUFFER_H
#define EXPLOSION_RHI_BUFFER_H

#include <Common/Utility.h>
#include <RHI/Enum.h>

namespace RHI {
    struct BufferCreateInfo {
        size_t size;
        BufferUsage usage;
        SharingMode sharingMode;
    };

    class Buffer {
    public:
        NON_COPYABLE(Buffer)
        virtual ~Buffer();

    protected:
        explicit Buffer(const BufferCreateInfo* createInfo);
    };
}

#endif //EXPLOSION_RHI_BUFFER_H
