//
// Created by johnk on 22/3/2022.
//

#pragma once

#include <cstdint>
#include <vector>
#include <unordered_map>

#include <wrl/client.h>
#include <directx/d3dx12.h>
using Microsoft::WRL::ComPtr;

#include <RHI/CommandBuffer.h>
#include <RHI/Common.h>
#include <Common/Memory.h>

namespace RHI::DirectX12 {
    class DX12Device;
    class DX12BindGroup;

    class RuntimeDescriptorHeap {
    public:
        RuntimeDescriptorHeap(DX12Device& inDevice, D3D12_DESCRIPTOR_HEAP_TYPE inHeapType, uint32_t inCapacity);
        ~RuntimeDescriptorHeap();

        void ResetUsed();
        ID3D12DescriptorHeap* GetDX12DescriptorHeap();
        CD3DX12_GPU_DESCRIPTOR_HANDLE NewGpuDescriptorHandle(CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle);

    private:
        DX12Device& device;
        uint32_t capacity;
        uint32_t used;
        uint32_t descriptorIncrementSize;
        D3D12_DESCRIPTOR_HEAP_TYPE dx12HeapType;
        ComPtr<ID3D12DescriptorHeap> dx12DescriptorHeap;
    };

    class RuntimeDescriptorHeaps {
    public:
        RuntimeDescriptorHeaps(DX12Device& inDevice);

        void ResetUsed();
        std::vector<ID3D12DescriptorHeap*> GetDX12DescriptorHeaps();
        CD3DX12_GPU_DESCRIPTOR_HANDLE NewGpuDescriptorHandle(HlslBindingRangeType rangeType, CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle);

    private:
        // TODO check this
        static constexpr uint32_t sampelrHeapCapacity = 1024;
        static constexpr uint32_t cbvSrvUavHeapCapacity = 10240;

        DX12Device& device;
        Common::UniqueRef<RuntimeDescriptorHeap> samplerHeap;
        Common::UniqueRef<RuntimeDescriptorHeap> cbvSrvUavHeap;
    };

    class DX12CommandBuffer : public CommandBuffer {
    public:
        NON_COPYABLE(DX12CommandBuffer)
        explicit DX12CommandBuffer(DX12Device& device);
        ~DX12CommandBuffer() override;

        CommandEncoder* Begin() override;
        void Destroy() override;

        ComPtr<ID3D12GraphicsCommandList>& GetDX12GraphicsCommandList();
        RuntimeDescriptorHeaps* GetRuntimeDescriptorHeaps();

    private:
        void AllocateDX12CommandList(DX12Device& device);

        DX12Device& device;
        ComPtr<ID3D12GraphicsCommandList> dx12GraphicsCommandList;
        Common::UniqueRef<RuntimeDescriptorHeaps> runtimeDescriptorHeaps;
    };
}
