//
// Created by johnk on 3/1/2022.
//

#include <RHI/DirectX12/SwapChain.h>
#include <RHI/DirectX12/Enum.h>

namespace RHI::DirectX12 {
    DX12SwapChain::DX12SwapChain(DX12Instance& instance, const SwapChainCreateInfo* createInfo) : SwapChain(createInfo)
    {
        CreateSwapChain(instance, createInfo);
    }

    DX12SwapChain::~DX12SwapChain() = default;

    ComPtr<IDXGISwapChain3>& DX12SwapChain::GetDX12SwapChain()
    {
        return dx12SwapChain;
    }

    void DX12SwapChain::CreateSwapChain(DX12Instance& instance, const SwapChainCreateInfo* createInfo)
    {
        DXGI_SWAP_CHAIN_DESC1 desc {};
        desc.BufferCount = createInfo->imageNum;
        desc.Width = createInfo->extent.x;
        desc.Height = createInfo->extent.y;
        desc.Format = EnumCast<PixelFormat, DXGI_FORMAT>(createInfo->format);
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        // TODO
    }
}
