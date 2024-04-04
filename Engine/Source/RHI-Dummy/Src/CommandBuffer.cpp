//
// Created by johnk on 2023/3/21.
//

#include <RHI/Dummy/CommandBuffer.h>
#include <RHI/Dummy/CommandRecorder.h>

namespace RHI::Dummy {
    DummyCommandBuffer::DummyCommandBuffer() = default;

    CommandRecorder* DummyCommandBuffer::Begin()
    {
        return new DummyCommandRecorder(*this);
    }

    void DummyCommandBuffer::Destroy()
    {
        delete this;
    }
}
