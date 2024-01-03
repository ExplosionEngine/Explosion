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
        uint32_t size;
        BufferUsageFlags usages;
        std::string debugName;

        bool operator==(const BufferCreateInfo& rhs) const
        {
            return size == rhs.size
                && usages == rhs.usages;
        }
    };

    class Buffer {
    public:
        NonCopyable(Buffer)
        virtual ~Buffer();

        const BufferCreateInfo& GetCreateInfo() const;
        virtual void* Map(MapMode mapMode, size_t offset, size_t length) = 0;
        virtual void UnMap() = 0;
        virtual BufferView* CreateBufferView(const BufferViewCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;

    protected:
        explicit Buffer(const BufferCreateInfo& inCreateInfo);

        BufferCreateInfo createInfo;
    };
}
