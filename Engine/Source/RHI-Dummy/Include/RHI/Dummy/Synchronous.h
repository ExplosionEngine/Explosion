//
// Created by johnk on 2023/3/21.
//

#pragma once

#include <RHI/Synchronous.h>

namespace RHI::Dummy {
    class DummyDevice;

    class DummyFence : public Fence {
    public:
        NON_COPYABLE(DummyFence)
        explicit DummyFence(DummyDevice& device);
        ~DummyFence() override;

        FenceStatus GetStatus() override;
        void Reset() override;
        void Wait() override;
        void Destroy() override;
    };
}
