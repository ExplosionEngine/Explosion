//
// Created by johnk on 22/3/2022.
//

#include <RHI/DirectX12/CommandBuffer.h>
#include <RHI/DirectX12/Common.h>
#include <RHI/DirectX12/Device.h>
#include <RHI/DirectX12/CommandEncoder.h>
#include <RHI/DirectX12/BindGroup.h>

namespace RHI::DirectX12 {
    RuntimeDescriptorHeap::RuntimeDescriptorHeap(DX12Device& inDevice, D3D12_DESCRIPTOR_HEAP_TYPE inHeapType, uint32_t inCapacity)
        : device(inDevice), dx12HeapType(inHeapType), capacity(inCapacity)
    {
        descriptorIncrementSize = device.GetDX12Device()->GetDescriptorHandleIncrementSize(inHeapType);

        D3D12_DESCRIPTOR_HEAP_DESC desc {};
        desc.NumDescriptors = capacity;
        desc.Type = dx12HeapType;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

        bool success = SUCCEEDED(device.GetDX12Device()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&dx12DescriptorHeap)));
        Assert(success);
    }

    RuntimeDescriptorHeap::~RuntimeDescriptorHeap() = default;

    void RuntimeDescriptorHeap::ResetUsed()
    {
        used = 0;
    }

    ID3D12DescriptorHeap* RuntimeDescriptorHeap::GetDX12DescriptorHeap()
    {
        return dx12DescriptorHeap.Get();
    }

    CD3DX12_GPU_DESCRIPTOR_HANDLE RuntimeDescriptorHeap::NewGpuDescriptorHandle(CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle)
    {
        Assert(used + 1 < capacity);

        uint32_t srcDescriptorNum = 1;
        CD3DX12_CPU_DESCRIPTOR_HANDLE srcDescriptorStart = cpuHandle;

        uint32_t destDescriptorNum = 1;
        CD3DX12_CPU_DESCRIPTOR_HANDLE destDescriptorStart(dx12DescriptorHeap->GetCPUDescriptorHandleForHeapStart());
        destDescriptorStart.Offset(static_cast<int32_t>(used), descriptorIncrementSize);

        device.GetDX12Device()->CopyDescriptors(1, &destDescriptorStart, &destDescriptorNum, 1, &srcDescriptorStart, &srcDescriptorNum, dx12HeapType);
        CD3DX12_GPU_DESCRIPTOR_HANDLE result(dx12DescriptorHeap->GetGPUDescriptorHandleForHeapStart());
        result.Offset(static_cast<int32_t>(used), descriptorIncrementSize);
        used++;
        return result;
    }

    RuntimeDescriptorHeaps::RuntimeDescriptorHeaps(DX12Device& inDevice)
        : device(inDevice)
    {
        cbvSrvUavHeap = Common::MakeUnique<RuntimeDescriptorHeap>(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, cbvSrvUavHeapCapacity);
        samplerHeap = Common::MakeUnique<RuntimeDescriptorHeap>(device, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, sampelrHeapCapacity);
    }

    void RuntimeDescriptorHeaps::ResetUsed()
    {
        cbvSrvUavHeap->ResetUsed();
        samplerHeap->ResetUsed();
    }

    std::vector<ID3D12DescriptorHeap*> RuntimeDescriptorHeaps::GetDX12DescriptorHeaps()
    {
        return { cbvSrvUavHeap->GetDX12DescriptorHeap(), samplerHeap->GetDX12DescriptorHeap() };
    }

    CD3DX12_GPU_DESCRIPTOR_HANDLE RuntimeDescriptorHeaps::NewGpuDescriptorHandle(HlslBindingRangeType rangeType, CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle)
    {
        if (rangeType == HlslBindingRangeType::sampler) {
            return samplerHeap->NewGpuDescriptorHandle(cpuHandle);
        } else {
            return cbvSrvUavHeap->NewGpuDescriptorHandle(cpuHandle);
        }
    }

    DX12CommandBuffer::DX12CommandBuffer(DX12Device& device) : CommandBuffer(), device(device)
    {
        AllocateDX12CommandList(device);
        runtimeDescriptorHeaps = Common::MakeUnique<RuntimeDescriptorHeaps>(device);
    }

    DX12CommandBuffer::~DX12CommandBuffer() = default;

    CommandEncoder* DX12CommandBuffer::Begin()
    {
        return new DX12CommandEncoder(device, *this);
    }

    void DX12CommandBuffer::Destroy()
    {
        delete this;
    }

    ComPtr<ID3D12GraphicsCommandList>& DX12CommandBuffer::GetDX12GraphicsCommandList()
    {
        return dx12GraphicsCommandList;
    }

    RuntimeDescriptorHeaps* DX12CommandBuffer::GetRuntimeDescriptorHeaps()
    {
        return runtimeDescriptorHeaps.Get();
    }

    void DX12CommandBuffer::AllocateDX12CommandList(DX12Device& device)
    {
        bool success = SUCCEEDED(device.GetDX12Device()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, device.GetDX12CommandAllocator().Get(), nullptr, IID_PPV_ARGS(&dx12GraphicsCommandList)));
        Assert(success);
        dx12GraphicsCommandList->Close();
    }
}
