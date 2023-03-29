//
// Created by johnk on 28/3/2022.
//

#pragma once

#include <memory>

#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi1_4.h>
using Microsoft::WRL::ComPtr;

#include <RHI/SwapChain.h>

namespace RHI::DirectX12 {
    class DX12Device;
    class DX12Texture;

    class DX12SwapChain : public SwapChain {
    public:
        NON_COPYABLE(DX12SwapChain)
        explicit DX12SwapChain(DX12Device& device, const SwapChainCreateInfo& createInfo);
        ~DX12SwapChain() override;

        Texture* GetTexture(uint8_t index) override;
        uint8_t AcquireBackTexture() override;
        void Present() override;
        void Destroy() override;

    private:
        void CreateDX12SwapChain(const SwapChainCreateInfo& createInfo) ;
        void FetchTextures(PixelFormat format);

        DX12Device& device;
        uint8_t textureNum;
        PresentMode presentMode;
        ComPtr<IDXGISwapChain3> dx12SwapChain;
        std::vector<std::unique_ptr<Texture>> textures;
    };
}
