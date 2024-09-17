//
// Created by johnk on 15/1/2022.
//

#pragma once

#include <vector>
#include <unordered_map>

#include <wrl/client.h>
#include <directx/d3dx12.h>

#include <RHI/Synchronous.h>
#include <RHI/Common.h>
#include <RHI/Device.h>

using Microsoft::WRL::ComPtr;

namespace RHI::DirectX12 {
    class DX12Gpu;
    class DX12Queue;
    class DX12Device;

    class DescriptorHeapNode;

    class DescriptorAllocation {
    public:
        DescriptorAllocation(DescriptorHeapNode* inNode, uint32_t inSlot, const CD3DX12_CPU_DESCRIPTOR_HANDLE& inHandle, ID3D12DescriptorHeap* inHeap);
        ~DescriptorAllocation();

        const CD3DX12_CPU_DESCRIPTOR_HANDLE& GetCpuHandle() const;
        ID3D12DescriptorHeap* GetNativeDescriptorHeap() const;

    private:
        DescriptorHeapNode* node;
        uint32_t slot;
        CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle;
        ID3D12DescriptorHeap* nativeDescriptorHeap;
    };

    class DescriptorHeapNode {
    public:
        DescriptorHeapNode(ComPtr<ID3D12DescriptorHeap>&& inHeap, uint32_t inDescriptorSize, uint32_t inCapacity);

        bool HasFreeSlot() const;
        Common::UniqueRef<DescriptorAllocation> Allocate();
        void Free(uint32_t slot);

    private:
        ComPtr<ID3D12DescriptorHeap> nativeDescriptorHeap;
        uint32_t descriptorSize;
        uint32_t capacity;
        uint32_t free;
        std::vector<uint8_t> usedBitMasks;
    };

    class DescriptorPool {
    public:
        DescriptorPool(DX12Device& inDevice, D3D12_DESCRIPTOR_HEAP_TYPE inNativeHeapType, uint32_t inDescriptorSize, uint32_t inCapacity);
        Common::UniqueRef<DescriptorAllocation> Allocate();

    private:
        DX12Device& device;
        D3D12_DESCRIPTOR_HEAP_TYPE nativeHeapType;
        uint32_t descriptorSize;
        uint32_t capacity;
        std::list<DescriptorHeapNode> heapNodes;
    };

    class DX12Device final : public Device {
    public:
        NonCopyable(DX12Device)
        DX12Device(DX12Gpu& inGpu, const DeviceCreateInfo& inCreateInfo);
        ~DX12Device() override;

        size_t GetQueueNum(QueueType inType) override;
        Queue* GetQueue(QueueType inType, size_t inIndex) override;
        Common::UniqueRef<Surface> CreateSurface(const SurfaceCreateInfo& inCreateInfo) override;
        Common::UniqueRef<SwapChain> CreateSwapChain(const SwapChainCreateInfo& inCreateInfo) override;
        Common::UniqueRef<Buffer> CreateBuffer(const BufferCreateInfo& inCreateInfo) override;
        Common::UniqueRef<Texture> CreateTexture(const TextureCreateInfo& inCreateInfo) override;
        Common::UniqueRef<Sampler> CreateSampler(const SamplerCreateInfo& inCreateInfo) override;
        Common::UniqueRef<BindGroupLayout> CreateBindGroupLayout(const BindGroupLayoutCreateInfo& inCreateInfo) override;
        Common::UniqueRef<BindGroup> CreateBindGroup(const BindGroupCreateInfo& inCreateInfo) override;
        Common::UniqueRef<PipelineLayout> CreatePipelineLayout(const PipelineLayoutCreateInfo& inCreateInfo) override;
        Common::UniqueRef<ShaderModule> CreateShaderModule(const ShaderModuleCreateInfo& inCreateInfo) override;
        Common::UniqueRef<ComputePipeline> CreateComputePipeline(const ComputePipelineCreateInfo& inCreateInfo) override;
        Common::UniqueRef<RasterPipeline> CreateRasterPipeline(const RasterPipelineCreateInfo& inCreateInfo) override;
        Common::UniqueRef<CommandBuffer> CreateCommandBuffer() override;
        Common::UniqueRef<Fence> CreateFence(bool inInitAsSignaled) override;
        Common::UniqueRef<Semaphore> CreateSemaphore() override;

        bool CheckSwapChainFormatSupport(Surface* inSurface, PixelFormat inFormat) override;
        TextureSubResourceCopyFootprint GetTextureSubResourceCopyFootprint(const Texture& texture, const TextureSubResourceInfo& subResourceInfo) override;

        DX12Gpu& GetGpu() const;
        ID3D12Device* GetNative() const;
        ID3D12CommandAllocator* GetNativeCmdAllocator() const;
        Common::UniqueRef<DescriptorAllocation> AllocateRtvDescriptor() const;
        Common::UniqueRef<DescriptorAllocation> AllocateCbvSrvUavDescriptor() const;
        Common::UniqueRef<DescriptorAllocation> AllocateSamplerDescriptor() const;
        Common::UniqueRef<DescriptorAllocation> AllocateDsvDescriptor() const;

    private:
        void CreateNativeDevice();
        void CreateNativeQueues(const DeviceCreateInfo& inCreateInfo);
        void CreateNativeCmdAllocator();
        void QueryNativeDescriptorSize();
        void CreateDescriptorPools();
#if BUILD_CONFIG_DEBUG
        void RegisterNativeDebugLayerExceptionHandler() const;
        void UnregisterNativeDebugLayerExceptionHandler() const;
#endif

        DX12Gpu& gpu;
        std::unordered_map<QueueType, std::vector<Common::UniqueRef<DX12Queue>>> queues;
        uint32_t nativeRtvDescriptorSize;
        uint32_t nativeCbvSrvUavDescriptorSize;
        uint32_t nativeSamplerDescriptorSize;
        uint32_t nativeDsvDescriptorSize;
        Common::UniqueRef<DescriptorPool> rtvDescriptorPool;
        Common::UniqueRef<DescriptorPool> cbvSrvUavDescriptorPool;
        Common::UniqueRef<DescriptorPool> samplerDescriptorPool;
        Common::UniqueRef<DescriptorPool> dsvDescriptorPool;
        ComPtr<ID3D12Device> nativeDevice;
        ComPtr<ID3D12CommandAllocator> nativeCmdAllocator;
    };
}
