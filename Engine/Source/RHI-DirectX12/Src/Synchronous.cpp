//
// Created by johnk on 2022/5/15.
//

#include <Common/Debug.h>
#include <RHI/DirectX12/Synchronous.h>
#include <RHI/DirectX12/Device.h>

namespace RHI::DirectX12 {
    DX12Fence::DX12Fence(DX12Device& inDevice, const bool inInitAsSignaled)
        : Fence(inDevice, inInitAsSignaled)
        , nativeFenceEvent(nullptr)
    {
        CreateNativeFence(inDevice, inInitAsSignaled);
        CreateNativeFenceEvent();
    }

    DX12Fence::~DX12Fence()
    {
        CloseHandle(nativeFenceEvent);
    }

    bool DX12Fence::IsSignaled()
    {
        return nativeFence->GetCompletedValue() == 1;
    }

    void DX12Fence::Reset()
    {
        nativeFence->Signal(0);
    }

    void DX12Fence::Wait()
    {
        Assert(SUCCEEDED(nativeFence->SetEventOnCompletion(1, nativeFenceEvent)));
        WaitForSingleObject(nativeFenceEvent, INFINITE);
    }

    ID3D12Fence* DX12Fence::GetNative() const
    {
        return nativeFence.Get();
    }

    void DX12Fence::CreateNativeFence(DX12Device& inDevice, const bool inInitAsSignaled) // NOLINT
    {
        Assert(SUCCEEDED(inDevice.GetNative()->CreateFence(inInitAsSignaled ? 1 : 0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&nativeFence))));
    }

    void DX12Fence::CreateNativeFenceEvent()
    {
        nativeFenceEvent = CreateEvent(nullptr, false, false, nullptr);
        Assert(nativeFenceEvent);
    }

    DX12Semaphore::DX12Semaphore(DX12Device& inDevice)
        : Semaphore(inDevice)
    {
        CreateNativeFence(inDevice);
    }

    ID3D12Fence* DX12Semaphore::GetNative() const
    {
        return dx12Fence.Get();
    }

    void DX12Semaphore::CreateNativeFence(DX12Device& inDevice) // NOLINT
    {
        Assert(SUCCEEDED(inDevice.GetNative()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&dx12Fence))));
    }
}
