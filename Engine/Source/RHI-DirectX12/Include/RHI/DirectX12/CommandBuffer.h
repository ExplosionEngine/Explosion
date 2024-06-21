//
// Created by johnk on 22/3/2022.
//

#pragma once

#include <cstdint>
#include <vector>

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
        CD3DX12_GPU_DESCRIPTOR_HANDLE NewGpuDescriptorHandle(CD3DX12_CPU_DESCRIPTOR_HANDLE inCpuHandle);
        ID3D12DescriptorHeap* GetNative() const;

    private:
        DX12Device& device;
        uint32_t capacity;
        uint32_t used;
        uint32_t descriptorIncrementSize;
        D3D12_DESCRIPTOR_HEAP_TYPE nativeHeapType;
        ComPtr<ID3D12DescriptorHeap> nativeDescriptorHeap;
    };

    class RuntimeDescriptorCompact {
    public:
        explicit RuntimeDescriptorCompact(DX12Device& inDevice);

        void ResetUsed() const;
        CD3DX12_GPU_DESCRIPTOR_HANDLE NewGpuDescriptorHandle(HlslBindingRangeType inRangeType, CD3DX12_CPU_DESCRIPTOR_HANDLE inCpuHandle) const;
        std::vector<ID3D12DescriptorHeap*> GetNative() const;

    private:
        static constexpr uint32_t samplerHeapCapacity = 1024;
        static constexpr uint32_t cbvSrvUavHeapCapacity = 10240;

        DX12Device& device;
        Common::UniqueRef<RuntimeDescriptorHeap> samplerHeap;
        Common::UniqueRef<RuntimeDescriptorHeap> cbvSrvUavHeap;
    };

    class DX12CommandBuffer final : public CommandBuffer {
    public:
        NonCopyable(DX12CommandBuffer)
        explicit DX12CommandBuffer(DX12Device& inDevice);
        ~DX12CommandBuffer() override;

        Common::UniqueRef<CommandRecorder> Begin() override;

        ID3D12GraphicsCommandList* GetNative() const;
        RuntimeDescriptorCompact* GetRuntimeDescriptorHeaps() const;

    private:
        void AllocateDX12CommandList(DX12Device& inDevice);

        DX12Device& device;
        ComPtr<ID3D12GraphicsCommandList> dx12GraphicsCommandList;
        Common::UniqueRef<RuntimeDescriptorCompact> runtimeDescriptorHeaps;
    };
}
