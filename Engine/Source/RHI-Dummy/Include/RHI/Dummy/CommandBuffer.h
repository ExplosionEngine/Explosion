//
// Created by johnk on 2023/3/21.
//

#pragma once

#include <RHI/CommandBuffer.h>

namespace RHI::Dummy {
    class DummyCommandBuffer final : public CommandBuffer {
    public:
        NonCopyable(DummyCommandBuffer)
        DummyCommandBuffer();

        Common::UniqueRef<CommandRecorder> Begin() override;
    };
}
