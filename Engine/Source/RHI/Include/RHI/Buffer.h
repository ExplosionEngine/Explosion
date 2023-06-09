//
// Created by johnk on 2022/1/23.
//

#pragma once

#include <Common/Utility.h>
#include <RHI/Common.h>
#include <string>

namespace RHI {
    struct BufferViewCreateInfo;
    class BufferView;

    struct BufferCreateInfo {
        size_t size;
        BufferUsageFlags usages;
        std::string debugName;
    };

    class Buffer {
    public:
        NON_COPYABLE(Buffer)
        virtual ~Buffer();

        virtual void* Map(MapMode mapMode, size_t offset, size_t length) = 0;
        virtual void UnMap() = 0;
        virtual BufferView* CreateBufferView(const BufferViewCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;

    protected:
        explicit Buffer(const BufferCreateInfo& createInfo);
    };
}
