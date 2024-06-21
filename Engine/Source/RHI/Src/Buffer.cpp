//
// Created by johnk on 2022/1/23.
//

#include <RHI/Buffer.h>

namespace RHI {
    BufferCreateInfo::BufferCreateInfo() = default;

    BufferCreateInfo::BufferCreateInfo(const uint32_t inSize, const BufferUsageFlags inUsages, const BufferState inInitialState, std::string inDebugName)
        : size(inSize)
        , usages(inUsages)
        , initialState(inInitialState)
        , debugName(std::move(inDebugName))
    {
    }

    BufferCreateInfo& BufferCreateInfo::SetSize(const uint32_t inSize)
    {
        size = inSize;
        return *this;
    }
    BufferCreateInfo& BufferCreateInfo::SetUsages(const BufferUsageFlags inUsages)
    {
        usages = inUsages;
        return *this;
    }
    BufferCreateInfo& BufferCreateInfo::SetInitialState(const BufferState inState)
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
