//
// Created by johnk on 28/3/2022.
//

#pragma once

#include <wrl/client.h>
#include <dxgi1_4.h>
using Microsoft::WRL::ComPtr;

#include <RHI/SwapChain.h>

namespace RHI::DirectX12 {
    class DX12Device;
    class DX12Texture;

    class DX12SwapChain final : public SwapChain {
    public:
        NonCopyable(DX12SwapChain)
        explicit DX12SwapChain(DX12Device& inDevice, const SwapChainCreateInfo& inCreateInfo);
        ~DX12SwapChain() override;

        uint8_t GetTextureNum() override;
        Texture* GetTexture(uint8_t inIndex) override;
        uint8_t AcquireBackTexture(Semaphore* inSignalSemaphore) override;
        void Present(RHI::Semaphore* inWaitSemaphore) override;

    private:
        void CreateDX12SwapChain(const SwapChainCreateInfo& inCreateInfo) ;
        void FetchTextures(const SwapChainCreateInfo& inCreateInfo);

        DX12Device& device;
        DX12Queue& queue;
        uint8_t textureNum;
        PresentMode presentMode;
        ComPtr<IDXGISwapChain3> nativeSwapChain;
        std::vector<Common::UniqueRef<Texture>> textures;
    };
}
