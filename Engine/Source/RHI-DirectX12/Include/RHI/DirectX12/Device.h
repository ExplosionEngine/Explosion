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
#include <RHI/DirectX12/Gpu.h>

using Microsoft::WRL::ComPtr;

namespace RHI::DirectX12 {
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
        Common::UniquePtr<DescriptorAllocation> Allocate();
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
        Common::UniquePtr<DescriptorAllocation> Allocate();

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

        DX12Gpu& GetGpu() const override;
        size_t GetQueueNum(QueueType inType) override;
        Queue* GetQueue(QueueType inType, size_t inIndex) override;
        Common::UniquePtr<Surface> CreateSurface(const SurfaceCreateInfo& inCreateInfo) override;
        Common::UniquePtr<SwapChain> CreateSwapChain(const SwapChainCreateInfo& inCreateInfo) override;
        Common::UniquePtr<Buffer> CreateBuffer(const BufferCreateInfo& inCreateInfo) override;
        Common::UniquePtr<Texture> CreateTexture(const TextureCreateInfo& inCreateInfo) override;
        Common::UniquePtr<Sampler> CreateSampler(const SamplerCreateInfo& inCreateInfo) override;
        Common::UniquePtr<BindGroupLayout> CreateBindGroupLayout(const BindGroupLayoutCreateInfo& inCreateInfo) override;
        Common::UniquePtr<BindGroup> CreateBindGroup(const BindGroupCreateInfo& inCreateInfo) override;
        Common::UniquePtr<PipelineLayout> CreatePipelineLayout(const PipelineLayoutCreateInfo& inCreateInfo) override;
        Common::UniquePtr<ShaderModule> CreateShaderModule(const ShaderModuleCreateInfo& inCreateInfo) override;
        Common::UniquePtr<ComputePipeline> CreateComputePipeline(const ComputePipelineCreateInfo& inCreateInfo) override;
        Common::UniquePtr<RasterPipeline> CreateRasterPipeline(const RasterPipelineCreateInfo& inCreateInfo) override;
        Common::UniquePtr<CommandBuffer> CreateCommandBuffer() override;
        Common::UniquePtr<Fence> CreateFence(bool inInitAsSignaled) override;
        Common::UniquePtr<Semaphore> CreateSemaphore() override;

        bool CheckSwapChainFormatSupport(Surface* inSurface, PixelFormat inFormat) override;
        TextureSubResourceCopyFootprint GetTextureSubResourceCopyFootprint(const Texture& texture, const TextureSubResourceInfo& subResourceInfo) override;

        ID3D12Device* GetNative() const;
        Common::UniquePtr<DescriptorAllocation> AllocateRtvDescriptor() const;
        Common::UniquePtr<DescriptorAllocation> AllocateCbvSrvUavDescriptor() const;
        Common::UniquePtr<DescriptorAllocation> AllocateSamplerDescriptor() const;
        Common::UniquePtr<DescriptorAllocation> AllocateDsvDescriptor() const;

    private:
        void CreateNativeDevice();
        void CreateNativeQueues(const DeviceCreateInfo& inCreateInfo);
        void QueryNativeDescriptorSize();
        void CreateDescriptorPools();
#if BUILD_CONFIG_DEBUG
        void RegisterNativeDebugLayerExceptionHandler();
        void UnregisterNativeDebugLayerExceptionHandler();
        bool debugLayerExceptionHandlerReady;
#endif

        DX12Gpu& gpu;
        std::unordered_map<QueueType, std::vector<Common::UniquePtr<DX12Queue>>> queues;
        uint32_t nativeRtvDescriptorSize;
        uint32_t nativeCbvSrvUavDescriptorSize;
        uint32_t nativeSamplerDescriptorSize;
        uint32_t nativeDsvDescriptorSize;
        Common::UniquePtr<DescriptorPool> rtvDescriptorPool;
        Common::UniquePtr<DescriptorPool> cbvSrvUavDescriptorPool;
        Common::UniquePtr<DescriptorPool> samplerDescriptorPool;
        Common::UniquePtr<DescriptorPool> dsvDescriptorPool;
        ComPtr<ID3D12Device> nativeDevice;
    };
}
