//
// Created by johnk on 2023/3/21.
//

#pragma once

#include <vector>

#include <RHI/SwapChain.h>

namespace RHI::Dummy {
    class DummyTexture;

    class DummySwapChain : public SwapChain {
    public:
        NonCopyable(DummySwapChain)
        explicit DummySwapChain(const SwapChainCreateInfo& createInfo);
        ~DummySwapChain() override;

        Texture* GetTexture(uint8_t index) override;
        uint8_t AcquireBackTexture(RHI::Semaphore* signalSemaphore) override;
        void Present(RHI::Semaphore* waitSemaphore) override;

    private:
        bool pingPong;
        std::vector<Common::UniqueRef<DummyTexture>> dummyTextures;
    };
}
