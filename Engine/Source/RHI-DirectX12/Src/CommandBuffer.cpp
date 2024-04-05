//
// Created by johnk on 22/3/2022.
//

#include <RHI/DirectX12/CommandBuffer.h>
#include <RHI/DirectX12/Common.h>
#include <RHI/DirectX12/Device.h>
#include <RHI/DirectX12/CommandRecorder.h>
#include <RHI/DirectX12/BindGroup.h>

namespace RHI::DirectX12 {
    RuntimeDescriptorHeap::RuntimeDescriptorHeap(DX12Device& inDevice, D3D12_DESCRIPTOR_HEAP_TYPE inHeapType, uint32_t inCapacity)
        : device(inDevice), nativeHeapType(inHeapType), capacity(inCapacity)
    {
        descriptorIncrementSize = device.GetNative()->GetDescriptorHandleIncrementSize(inHeapType);

        D3D12_DESCRIPTOR_HEAP_DESC desc {};
        desc.NumDescriptors = capacity;
        desc.Type = nativeHeapType;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

        bool success = SUCCEEDED(device.GetNative()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&nativeDescriptorHeap)));
        Assert(success);
    }

    RuntimeDescriptorHeap::~RuntimeDescriptorHeap() = default;

    void RuntimeDescriptorHeap::ResetUsed()
    {
        used = 0;
    }

    ID3D12DescriptorHeap* RuntimeDescriptorHeap::GetNative()
    {
        return nativeDescriptorHeap.Get();
    }

    CD3DX12_GPU_DESCRIPTOR_HANDLE RuntimeDescriptorHeap::NewGpuDescriptorHandle(CD3DX12_CPU_DESCRIPTOR_HANDLE inCpuHandle)
    {
        Assert(used + 1 < capacity);

        uint32_t srcDescriptorNum = 1;
        CD3DX12_CPU_DESCRIPTOR_HANDLE srcDescriptorStart = inCpuHandle;

        uint32_t destDescriptorNum = 1;
        CD3DX12_CPU_DESCRIPTOR_HANDLE destDescriptorStart(nativeDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
        destDescriptorStart.Offset(static_cast<int32_t>(used), descriptorIncrementSize);

        device.GetNative()->CopyDescriptors(1, &destDescriptorStart, &destDescriptorNum, 1, &srcDescriptorStart, &srcDescriptorNum, nativeHeapType);
        CD3DX12_GPU_DESCRIPTOR_HANDLE result(nativeDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
        result.Offset(static_cast<int32_t>(used), descriptorIncrementSize);
        used++;
        return result;
    }

    RuntimeDescriptorCompact::RuntimeDescriptorCompact(DX12Device& inDevice)
        : device(inDevice)
    {
        cbvSrvUavHeap = Common::MakeUnique<RuntimeDescriptorHeap>(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, cbvSrvUavHeapCapacity);
        samplerHeap = Common::MakeUnique<RuntimeDescriptorHeap>(device, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, samplerHeapCapacity);
    }

    void RuntimeDescriptorCompact::ResetUsed()
    {
        cbvSrvUavHeap->ResetUsed();
        samplerHeap->ResetUsed();
    }

    std::vector<ID3D12DescriptorHeap*> RuntimeDescriptorCompact::GetNative()
    {
        return {cbvSrvUavHeap->GetNative(), samplerHeap->GetNative() };
    }

    CD3DX12_GPU_DESCRIPTOR_HANDLE RuntimeDescriptorCompact::NewGpuDescriptorHandle(HlslBindingRangeType inRangeType, CD3DX12_CPU_DESCRIPTOR_HANDLE inCpuHandle)
    {
        if (inRangeType == HlslBindingRangeType::sampler) {
            return samplerHeap->NewGpuDescriptorHandle(inCpuHandle);
        } else {
            return cbvSrvUavHeap->NewGpuDescriptorHandle(inCpuHandle);
        }
    }

    DX12CommandBuffer::DX12CommandBuffer(DX12Device& inDevice) : CommandBuffer(), device(inDevice)
    {
        AllocateDX12CommandList(inDevice);
        runtimeDescriptorHeaps = Common::MakeUnique<RuntimeDescriptorCompact>(inDevice);
    }

    DX12CommandBuffer::~DX12CommandBuffer() = default;

    CommandRecorder* DX12CommandBuffer::Begin()
    {
        return new DX12CommandRecorder(device, *this);
    }

    void DX12CommandBuffer::Destroy()
    {
        delete this;
    }

    ID3D12GraphicsCommandList* DX12CommandBuffer::GetNative()
    {
        return dx12GraphicsCommandList.Get();
    }

    RuntimeDescriptorCompact* DX12CommandBuffer::GetRuntimeDescriptorHeaps()
    {
        return runtimeDescriptorHeaps.Get();
    }

    void DX12CommandBuffer::AllocateDX12CommandList(DX12Device& inDevice)
    {
        bool success = SUCCEEDED(inDevice.GetNative()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, inDevice.GetNativeCmdAllocator(), nullptr, IID_PPV_ARGS(&dx12GraphicsCommandList)));
        Assert(success);
        dx12GraphicsCommandList->Close();
    }
}
