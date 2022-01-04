//
// Created by johnk on 2/1/2022.
//

#ifndef EXPLOSION_RHI_DX12_SWAP_CHAIN_H
#define EXPLOSION_RHI_DX12_SWAP_CHAIN_H

#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi1_4.h>

#include <RHI/SwapChain.h>

using namespace Microsoft::WRL;

namespace RHI::DirectX12 {
    class DX12Instance;
    class DX12LogicalDevice;
    class DX12DeviceMemory;
    class DX12Image;

    class DX12SwapChain : public SwapChain {
    public:
        NON_COPYABLE(DX12SwapChain)
        explicit DX12SwapChain(DX12Instance& instance, const SwapChainCreateInfo* createInfo);
        ~DX12SwapChain() override;

        size_t GetImageNum() override;
        Image* GetImage(size_t index) override;

        ComPtr<IDXGISwapChain3>& GetDX12SwapChain();

    private:
        void CreateSwapChain(DX12Instance& instance, const SwapChainCreateInfo* createInfo);
        void FetchImages(const SwapChainCreateInfo* createInfo);

        ComPtr<IDXGISwapChain3> dx12SwapChain;
        std::vector<std::unique_ptr<DX12Image>> frameImages;
        std::vector<std::unique_ptr<DX12DeviceMemory>> frameMemories;
    };
}

#endif //EXPLOSION_RHI_DX12_SWAP_CHAIN_H
