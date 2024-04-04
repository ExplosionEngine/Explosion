//
// Created by johnk on 2022/1/23.
//

#include <RHI/Buffer.h>

namespace RHI {
    BufferCreateInfo::BufferCreateInfo() = default;

    BufferCreateInfo::BufferCreateInfo(uint32_t inSize, BufferUsageFlags inUsages, BufferState inInitialState, std::string inDebugName)
        : size(inSize)
        , usages(inUsages)
        , initialState(inInitialState)
        , debugName(std::move(inDebugName))
    {
    }

    BufferCreateInfo& BufferCreateInfo::SetSize(uint32_t inSize)
    {
        size = inSize;
        return *this;
    }
    BufferCreateInfo& BufferCreateInfo::SetUsages(BufferUsageFlags inUsages)
    {
        usages = inUsages;
        return *this;
    }
    BufferCreateInfo& BufferCreateInfo::SetInitialState(BufferState inState)
    {
        initialState = inState;
        return *this;
    }

    BufferCreateInfo& BufferCreateInfo::SetDebugName(std::string inDebugName)
    {
        debugName = std::move(inDebugName);
        return *this;
    }

    bool BufferCreateInfo::operator==(const BufferCreateInfo& rhs) const
    {
        return size == rhs.size
            && usages == rhs.usages;
    }

    Buffer::Buffer(const BufferCreateInfo& inCreateInfo)
        : createInfo(inCreateInfo)
    {
    }

    Buffer::~Buffer() = default;

    const BufferCreateInfo& Buffer::GetCreateInfo() const
    {
        return createInfo;
    }
}
