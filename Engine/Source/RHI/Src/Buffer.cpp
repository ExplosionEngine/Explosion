//
// Created by johnk on 2022/1/23.
//

#include <RHI/Buffer.h>

namespace RHI {
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
