//
// Created by johnk on 2022/5/15.
//

#pragma once

#include <cstdint>

#define NOMINMAX
#include <windows.h>
#include <wrl/client.h>
#include <d3d12.h>
using namespace Microsoft::WRL;

#include <RHI/Synchronous.h>
#include <Common/Utility.h>

namespace RHI::DirectX12 {
    class DX12Device;

    class DX12Fence final : public Fence {
    public:
        NonCopyable(DX12Fence)
        explicit DX12Fence(DX12Device& inDevice, bool inInitAsSignaled);
        ~DX12Fence() override;

        bool IsSignaled() override;
        void Reset() override;
        void Wait() override;

        ID3D12Fence* GetNative() const;

    private:
        void CreateNativeFence(DX12Device& inDevice, bool inInitAsSignaled);
        void CreateNativeFenceEvent();

        ComPtr<ID3D12Fence> nativeFence;
        HANDLE nativeFenceEvent;
    };

    class DX12Semaphore final : public Semaphore {
    public:
        NonCopyable(DX12Semaphore)
        explicit DX12Semaphore(DX12Device& inDevice);

        ID3D12Fence* GetNative() const;

    private:
        void CreateNativeFence(DX12Device& inDevice);

        ComPtr<ID3D12Fence> dx12Fence;
    };
}
