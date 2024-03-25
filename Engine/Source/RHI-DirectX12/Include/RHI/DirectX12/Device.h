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

    struct DescriptorAllocation {
        CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle;
        ID3D12DescriptorHeap* descriptorHeap;
    };

    class DX12Device : public Device {
    public:
        NonCopyable(DX12Device)
        DX12Device(DX12Gpu& gpu, const DeviceCreateInfo& createInfo);
        ~DX12Device() override;

        void Destroy() override;
        size_t GetQueueNum(QueueType type) override;
        Queue* GetQueue(QueueType type, size_t index) override;
        Surface* CreateSurface(const RHI::SurfaceCreateInfo &createInfo) override;
        SwapChain* CreateSwapChain(const SwapChainCreateInfo& createInfo) override;
        Buffer* CreateBuffer(const BufferCreateInfo& createInfo) override;
        Texture* CreateTexture(const TextureCreateInfo& createInfo) override;
        Sampler* CreateSampler(const SamplerCreateInfo& createInfo) override;
        BindGroupLayout* CreateBindGroupLayout(const BindGroupLayoutCreateInfo& createInfo) override;
        BindGroup* CreateBindGroup(const BindGroupCreateInfo& createInfo) override;
        PipelineLayout* CreatePipelineLayout(const PipelineLayoutCreateInfo& createInfo) override;
        ShaderModule* CreateShaderModule(const ShaderModuleCreateInfo& createInfo) override;
        ComputePipeline* CreateComputePipeline(const ComputePipelineCreateInfo& createInfo) override;
        GraphicsPipeline* CreateGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo) override;
        CommandBuffer* CreateCommandBuffer() override;
        Fence* CreateFence(bool initAsSignaled) override;

        bool CheckSwapChainFormatSupport(RHI::Surface* surface, PixelFormat format) override;

        DX12Gpu& GetGpu();
        ComPtr<ID3D12Device>& GetDX12Device();
        ComPtr<ID3D12CommandAllocator>& GetDX12CommandAllocator();
        DescriptorAllocation AllocateRtvDescriptor();
        DescriptorAllocation AllocateCbvSrvUavDescriptor();
        DescriptorAllocation AllocateSamplerDescriptor();
        DescriptorAllocation AllocateDsvDescriptor();

    private:
        struct DescriptorHeapListNode {
            uint8_t used;
            ComPtr<ID3D12DescriptorHeap> descriptorHeap;
        };

        inline DescriptorAllocation AllocateDescriptor(std::list<DescriptorHeapListNode>& list, uint8_t capacity, uint32_t descriptorSize, D3D12_DESCRIPTOR_HEAP_TYPE heapType);
        void CreateDX12Device();
        void CreateDX12Queues(const DeviceCreateInfo& createInfo);
        void CreateDX12CommandAllocator();
        void GetDX12DescriptorSize();
#if BUILD_CONFIG_DEBUG
        void RegisterDebugLayerExceptionHandler();
        void UnregisterDebugLayerExceptionHandler();
#endif

        DX12Gpu& gpu;
        std::unordered_map<QueueType, std::vector<Common::UniqueRef<DX12Queue>>> queues;
        uint32_t rtvDescriptorSize;
        uint32_t cbvSrvUavDescriptorSize;
        uint32_t samplerDescriptorSize;
        uint32_t dsvDescriptorSize;
        std::list<DescriptorHeapListNode> rtvHeapList;
        std::list<DescriptorHeapListNode> cbvSrvUavHeapList;
        std::list<DescriptorHeapListNode> samplerHeapList;
        std::list<DescriptorHeapListNode> dsvHeapList;
        ComPtr<ID3D12Device> dx12Device;
        ComPtr<ID3D12CommandAllocator> dx12CommandAllocator;
    };
}
