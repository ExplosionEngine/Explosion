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
        BufferState initialState;
        std::string debugName;

        BufferCreateInfo();
        BufferCreateInfo(uint32_t inSize, BufferUsageFlags inUsages, BufferState inInitialState, std::string inDebugName = "");

        BufferCreateInfo& SetSize(uint32_t inSize);
        BufferCreateInfo& SetUsages(BufferUsageFlags inUsages);
        BufferCreateInfo& SetInitialState(BufferState inState);
        BufferCreateInfo& SetDebugName(std::string inDebugName);

        bool operator==(const BufferCreateInfo& rhs) const;
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
