//
// Created by johnk on 2023/3/21.
//

#pragma once

#include <vector>
#include <memory>

#include <RHI/SwapChain.h>

namespace RHI::Dummy {
    class DummyTexture;

    class DummySwapChain : public SwapChain {
    public:
        explicit DummySwapChain(const SwapChainCreateInfo& createInfo);
        ~DummySwapChain() override;

        Texture* GetTexture(uint8_t index) override;
        uint8_t AcquireBackTexture() override;
        void Present() override;
        void Destroy() override;

    private:
        bool pingPong;
        std::vector<std::unique_ptr<DummyTexture>> dummyTextures;
    };
}
