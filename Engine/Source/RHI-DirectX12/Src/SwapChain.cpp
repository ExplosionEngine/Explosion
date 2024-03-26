//
// Created by johnk on 28/3/2022.
//

#include <Windows.h>

#include <RHI/DirectX12/Instance.h>
#include <RHI/DirectX12/Device.h>
#include <RHI/DirectX12/Gpu.h>
#include <RHI/DirectX12/SwapChain.h>
#include <RHI/DirectX12/Queue.h>
#include <RHI/DirectX12/Common.h>
#include <RHI/DirectX12/Texture.h>
#include <RHI/DirectX12/Surface.h>
#include <RHI/DirectX12/Synchronous.h>

namespace RHI::DirectX12 {
    static uint8_t GetSyncInterval(PresentMode presentMode)
    {
        return presentMode == PresentMode::immediately ? 1 : 0;
    }
}

namespace RHI::DirectX12 {
    DX12SwapChain::DX12SwapChain(DX12Device& device, const SwapChainCreateInfo& createInfo)
        : SwapChain(createInfo)
        , device(device)
        , queue(static_cast<DX12Queue&>(*createInfo.presentQueue))
        , presentMode(createInfo.presentMode)
        , textureNum(createInfo.textureNum)
    {
        CreateDX12SwapChain(createInfo);
        FetchTextures(createInfo.format);
    }

    DX12SwapChain::~DX12SwapChain() = default;

    Texture* DX12SwapChain::GetTexture(uint8_t index)
    {
        return textures[index].Get();
    }

    uint8_t DX12SwapChain::AcquireBackTexture(Semaphore* signalSemaphore)
    {
        auto& dx12SignalSemaphore = static_cast<DX12Semaphore&>(*signalSemaphore);
        auto result = static_cast<uint8_t>(dx12SwapChain->GetCurrentBackBufferIndex());
        auto& fence = dx12SignalSemaphore.GetDX12Fence();
        fence->Signal(0);
        fence->Signal(1);
        return result;
    }

    void DX12SwapChain::Present(Semaphore* waitSemaphore)
    {
        auto& dx12WaitSemaphore = static_cast<DX12Semaphore&>(*waitSemaphore);
        auto& fence = dx12WaitSemaphore.GetDX12Fence();
        dx12SwapChain->Present(GetSyncInterval(presentMode), false);
        queue.GetDX12CommandQueue()->Wait(fence.Get(), 1);
    }

    void DX12SwapChain::Destroy()
    {
        delete this;
    }

    void DX12SwapChain::CreateDX12SwapChain(const SwapChainCreateInfo& createInfo)
    {
        auto& instance = device.GetGpu().GetInstance();
        auto* dx12Queue = static_cast<DX12Queue*>(createInfo.presentQueue);
        Assert(dx12Queue != nullptr);
        auto* dx12Surface = static_cast<DX12Surface*>(createInfo.surface);
        Assert(dx12Surface != nullptr);

        DXGI_SWAP_CHAIN_DESC1 desc {};
        desc.BufferCount = createInfo.textureNum;
        desc.Width = createInfo.extent.x;
        desc.Height = createInfo.extent.y;
        desc.Format = DX12EnumCast<PixelFormat, DXGI_FORMAT>(createInfo.format);
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.SwapEffect = DX12EnumCast<PresentMode, DXGI_SWAP_EFFECT>(createInfo.presentMode);
        desc.SampleDesc.Count = 1;

        ComPtr<IDXGISwapChain1> dx12SwapChain1;
        bool success = SUCCEEDED(instance.GetDX12Factory()->CreateSwapChainForHwnd(
            dx12Queue->GetDX12CommandQueue().Get(),
            dx12Surface->GetWin32WindowHandle(),
            &desc,
            /* TODO fullscreen */ nullptr,
            nullptr,
            &dx12SwapChain1));
        Assert(success);

        success = SUCCEEDED(dx12SwapChain1.As(&dx12SwapChain));
        Assert(success);
    }

    void DX12SwapChain::FetchTextures(PixelFormat format)
    {
        textures.resize(textureNum);
        for (auto i = 0; i < textureNum; i++) {
            ComPtr<ID3D12Resource> dx12Resource;
            bool success = SUCCEEDED(dx12SwapChain->GetBuffer(i, IID_PPV_ARGS(&dx12Resource)));
            Assert(success);
            textures[i] = new DX12Texture(device, format, std::move(dx12Resource));
        }
    }
}
