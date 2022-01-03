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
    struct DX12Instance;
    struct DX12LogicalDevice;

    class DX12SwapChain : public SwapChain {
    public:
        NON_COPYABLE(DX12SwapChain)
        explicit DX12SwapChain(DX12Instance& instance, const SwapChainCreateInfo* createInfo);
        ~DX12SwapChain() override;

        ComPtr<IDXGISwapChain3>& GetDX12SwapChain();

    private:
        void CreateSwapChain(DX12Instance& instance, const SwapChainCreateInfo* createInfo);

        ComPtr<IDXGISwapChain3> dx12SwapChain;
    };
}

#endif //EXPLOSION_RHI_DX12_SWAP_CHAIN_H
