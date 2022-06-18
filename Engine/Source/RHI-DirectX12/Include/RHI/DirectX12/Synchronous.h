//
// Created by johnk on 2022/5/15.
//

#pragma once

#include <cstdint>

#include <windows.h>
#include <wrl/client.h>
#include <d3d12.h>
using namespace Microsoft::WRL;

#include <RHI/Synchronous.h>

namespace RHI::DirectX12 {
    class DX12Device;

    class DX12Fence : public Fence {
    public:
        explicit DX12Fence(DX12Device& device);
        ~DX12Fence();

        FenceStatus GetStatus() override;
        void Reset() override;
        void Wait() override;
        void Destroy() override;

        ComPtr<ID3D12Fence>& GetDX12Fence();

    private:
        void CreateDX12Fence(DX12Device& device);
        void CreateDX12FenceEvent();

        ComPtr<ID3D12Fence> dx12Fence;
        HANDLE dx12FenceEvent;
        bool signaled;
    };
}
