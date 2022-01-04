//
// Created by johnk on 3/1/2022.
//

#include <RHI/DirectX12/Instance.h>
#include <RHI/DirectX12/Queue.h>
#include <RHI/DirectX12/SwapChain.h>
#include <RHI/DirectX12/Surface.h>
#include <RHI/DirectX12/DeviceMemory.h>
#include <RHI/DirectX12/Image.h>
#include <RHI/DirectX12/Enum.h>

namespace RHI::DirectX12 {
    DX12SwapChain::DX12SwapChain(DX12Instance& instance, const SwapChainCreateInfo* createInfo) : SwapChain(createInfo)
    {
        CreateSwapChain(instance, createInfo);
        FetchImages(createInfo);
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
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        desc.SampleDesc.Count = 1;

        auto* queue = dynamic_cast<DX12Queue*>(createInfo->queue);
        if (queue == nullptr) {
            throw DX12Exception("must declare queue in swap chain create info");
        }
        auto* surface = dynamic_cast<DX12Surface*>(createInfo->surface);
        if (surface == nullptr) {
            throw DX12Exception("must declare surface in swap chain create info");
        }

        ComPtr<IDXGISwapChain1> sc;
        ThrowIfFailed(
            instance.GetDXGIFactory()->CreateSwapChainForHwnd(queue->GetDX12CommandQueue().Get(), surface->GetHWND(), &desc, nullptr, nullptr, &sc),
            "failed to create dx12 swap chain"
        );
        ThrowIfFailed(
            sc.As(&dx12SwapChain),
            "failed to cast dx12 swap chain"
        );
    }

    void DX12SwapChain::FetchImages(const SwapChainCreateInfo* createInfo)
    {
        frameMemories.resize(createInfo->imageNum);
        frameImages.resize(createInfo->imageNum);
        for (auto i = 0; i < createInfo->imageNum; i++) {
            ComPtr<ID3D12Resource> frameResource;
            ThrowIfFailed(
                dx12SwapChain->GetBuffer(i, IID_PPV_ARGS(&frameResource)),
                "failed to fetch dx12 images from swap chain"
            );
            frameMemories[i] = std::make_unique<DX12DeviceMemory>(std::move(frameResource));
            frameImages[i] = std::make_unique<DX12Image>(frameMemories[i].get());
        }
    }

    size_t DX12SwapChain::GetImageNum()
    {
        return frameImages.size();
    }

    Image* DX12SwapChain::GetImage(size_t index)
    {
        if (index < 0 || index >= frameImages.size()) {
            return nullptr;
        }
        return frameImages[index].get();
    }
}
