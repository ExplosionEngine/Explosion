//
// Created by johnk on 2023/3/21.
//

#include <RHI/Dummy/CommandBuffer.h>
#include <RHI/Dummy/CommandRecorder.h>

namespace RHI::Dummy {
    DummyCommandBuffer::DummyCommandBuffer() = default;

    Common::UniqueRef<CommandRecorder> DummyCommandBuffer::Begin()
    {
        return Common::UniqueRef<CommandRecorder>(new DummyCommandRecorder(*this));
    }
}
