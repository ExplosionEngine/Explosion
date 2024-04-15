//
// Created by johnk on 15/1/2022.
//

#pragma once

#include <vector>
#include <unordered_map>

#include <wrl/client.h>
#include <d3d12.h>
#include <directx/d3dx12.h>

#include <RHI/Synchronous.h>
#include <RHI/Common.h>
#include <RHI/Device.h>

using Microsoft::WRL::ComPtr;

namespace RHI::DirectX12 {
    class DX12Gpu;
    class DX12Queue;

    struct NativeDescriptorAllocation {
        CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle;
        ID3D12DescriptorHeap* descriptorHeap;
    };

    class DX12Device : public Device {
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

        bool CheckSwapChainFormatSupport(RHI::Surface* inSurface, PixelFormat inFormat) override;

        DX12Gpu& GetGpu();
        ID3D12Device* GetNative();
        ID3D12CommandAllocator* GetNativeCmdAllocator();
        NativeDescriptorAllocation AllocateNativeRtvDescriptor();
        NativeDescriptorAllocation AllocateNativeCbvSrvUavDescriptor();
        NativeDescriptorAllocation AllocateNativeSamplerDescriptor();
        NativeDescriptorAllocation AllocateNativeDsvDescriptor();

    private:
        struct NativeDescriptorHeapListNode {
            uint8_t used;
            ComPtr<ID3D12DescriptorHeap> descriptorHeap;
        };

        inline NativeDescriptorAllocation AllocateNativeDescriptor(std::list<NativeDescriptorHeapListNode>& inList, uint8_t inCapacity, uint32_t inDescriptorSize, D3D12_DESCRIPTOR_HEAP_TYPE inHeapType);
        void CreateNativeDevice();
        void CreateNativeQueues(const DeviceCreateInfo& inCreateInfo);
        void CreateNativeCmdAllocator();
        void QueryNativeDescriptorSize();
#if BUILD_CONFIG_DEBUG
        void RegisterNativeDebugLayerExceptionHandler();
        void UnregisterNativeDebugLayerExceptionHandler();
#endif

        DX12Gpu& gpu;
        std::unordered_map<QueueType, std::vector<Common::UniqueRef<DX12Queue>>> queues;
        uint32_t nativeRtvDescriptorSize;
        uint32_t nativeCbvSrvUavDescriptorSize;
        uint32_t nativeSamplerDescriptorSize;
        uint32_t nativeDsvDescriptorSize;
        std::list<NativeDescriptorHeapListNode> nativeRtvHeapList;
        std::list<NativeDescriptorHeapListNode> nativeCbvSrvUavHeapList;
        std::list<NativeDescriptorHeapListNode> nativeSamplerHeapList;
        std::list<NativeDescriptorHeapListNode> nativeDsvHeapList;
        ComPtr<ID3D12Device> nativeDevice;
        ComPtr<ID3D12CommandAllocator> nativeCmdAllocator;
    };
}
