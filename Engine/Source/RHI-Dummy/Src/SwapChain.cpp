//
// Created by johnk on 2023/3/21.
//

#include <RHI/Dummy/SwapChain.h>
#include <RHI/Dummy/Texture.h>
#include <Common/Debug.h>

namespace RHI::Dummy {
    DummySwapChain::DummySwapChain(const SwapChainCreateInfo& createInfo)
        : SwapChain(createInfo)
    {
        dummyTextures.reserve(2);
        for (auto i = 0; i < 2; i++) {
            dummyTextures.emplace_back(Common::MakeUnique<DummyTexture>());
        }
    }

    DummySwapChain::~DummySwapChain() = default;

    Texture* DummySwapChain::GetTexture(uint8_t index)
    {
        Assert(index < dummyTextures.size());
        return dummyTextures[index].Get();
    }

    uint8_t DummySwapChain::AcquireBackTexture(RHI::Semaphore* signalSemaphore)
    {
        pingPong = !pingPong;
        return pingPong ? 0 : 1;
    }

    void DummySwapChain::Present(RHI::Semaphore* waitSemaphore)
    {
    }
}
