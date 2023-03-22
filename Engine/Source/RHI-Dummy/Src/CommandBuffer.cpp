//
// Created by johnk on 2023/3/21.
//

#include <RHI/Dummy/CommandBuffer.h>
#include <RHI/Dummy/CommandEncoder.h>

namespace RHI::Dummy {
    DummyCommandBuffer::DummyCommandBuffer() = default;

    CommandEncoder* DummyCommandBuffer::Begin()
    {
        return new DummyCommandEncoder(*this);
    }

    void DummyCommandBuffer::Destroy()
    {
        delete this;
    }
}
