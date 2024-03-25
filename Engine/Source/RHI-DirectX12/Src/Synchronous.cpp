//
// Created by johnk on 2022/5/15.
//

#include <Common/Debug.h>
#include <RHI/DirectX12/Synchronous.h>
#include <RHI/DirectX12/Device.h>

namespace RHI::DirectX12 {
    DX12Fence::DX12Fence(DX12Device& device, bool initAsSignaled) : Fence(device, initAsSignaled), dx12FenceEvent(nullptr)
    {
        CreateDX12Fence(device, initAsSignaled);
        CreateDX12FenceEvent();
    }

    DX12Fence::~DX12Fence()
    {
        CloseHandle(dx12FenceEvent);
    }

    bool DX12Fence::IsSignaled()
    {
        return dx12Fence->GetCompletedValue() == 1;
    }

    void DX12Fence::Reset()
    {
        dx12Fence->Signal(0);
    }

    void DX12Fence::Wait()
    {
        Assert(SUCCEEDED(dx12Fence->SetEventOnCompletion(1, dx12FenceEvent)));
        WaitForSingleObject(dx12FenceEvent, INFINITE);
    }

    ComPtr<ID3D12Fence>& DX12Fence::GetDX12Fence()
    {
        return dx12Fence;
    }

    void DX12Fence::CreateDX12Fence(DX12Device& device, bool initAsSignaled)
    {
        Assert(SUCCEEDED(device.GetDX12Device()->CreateFence(initAsSignaled ? 1 : 0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&dx12Fence))));
    }

    void DX12Fence::CreateDX12FenceEvent()
    {
        dx12FenceEvent = CreateEvent(nullptr, false, false, nullptr);
        Assert(dx12FenceEvent);
    }

    void DX12Fence::Destroy()
    {
        delete this;
    }
}
