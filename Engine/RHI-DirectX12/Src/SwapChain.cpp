//
// Created by johnk on 28/3/2022.
//

#include <RHI/DirectX12/Instance.h>
#include <RHI/DirectX12/SwapChain.h>
#include <RHI/DirectX12/Queue.h>
#include <RHI/DirectX12/Common.h>
#include <RHI/DirectX12/Texture.h>

namespace RHI::DirectX12 {
    static uint8_t GetSyncInterval(PresentMode presentMode)
    {
        return presentMode == PresentMode::VSYNC ? 1 : 0;
    }
}

namespace RHI::DirectX12 {
    DX12SwapChain::DX12SwapChain(DX12Instance& instance, const SwapChainCreateInfo* createInfo) : SwapChain(createInfo), presentMode(createInfo->presentMode), textureNum(createInfo->textureNum)
    {
        CreateDX12SwapChain(instance, createInfo);
        FetchTextures();
    }

    DX12SwapChain::~DX12SwapChain() = default;

    Texture* DX12SwapChain::GetTexture(uint8_t index)
    {
        return textures[index].get();
    }

    uint8_t DX12SwapChain::GetBackTextureIndex()
    {
        return static_cast<uint8_t>(dx12SwapChain->GetCurrentBackBufferIndex());
    }

    void DX12SwapChain::Present()
    {
        dx12SwapChain->Present(GetSyncInterval(presentMode), false);
    }

    void DX12SwapChain::Destroy()
    {
        delete this;
    }

    void DX12SwapChain::CreateDX12SwapChain(DX12Instance& instance, const SwapChainCreateInfo* createInfo)
    {
        auto* dx12Queue = dynamic_cast<DX12Queue*>(createInfo->presentQueue);

        DXGI_SWAP_CHAIN_DESC1 desc {};
        desc.BufferCount = createInfo->textureNum;
        desc.Width = createInfo->extent.x;
        desc.Height = createInfo->extent.y;
        desc.Format = DX12EnumCast<PixelFormat, DXGI_FORMAT>(createInfo->format);
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.SwapEffect = DX12EnumCast<PresentMode, DXGI_SWAP_EFFECT>(createInfo->presentMode);
        desc.SampleDesc.Count = 1;

        ComPtr<IDXGISwapChain1> dx12SwapChain1;
        if (FAILED(instance.GetDX12Factory()->CreateSwapChainForHwnd(
            dx12Queue->GetDX12CommandQueue().Get(),
            createInfo->hWnd,
            &desc,
            /* TODO fullscreen */ nullptr,
            nullptr,
            &dx12SwapChain1
        ))) {
            throw DX12Exception("failed to create dx12 swap chain");
        }

        if (FAILED(dx12SwapChain1.As(&dx12SwapChain))) {
            throw DX12Exception("failed to cast IDXGISwapChain1 to IDXGISwapChain3");
        }
    }

    void DX12SwapChain::FetchTextures()
    {
        for (auto i = 0; i < textureNum; i++) {
            ComPtr<ID3D12Resource> dx12Resource;
            if (FAILED(dx12SwapChain->GetBuffer(i, IID_PPV_ARGS(&dx12Resource)))) {
                throw DX12Exception("failed to get dx12 resource from swap chain");
            }
            // TODO
        }
    }
}
