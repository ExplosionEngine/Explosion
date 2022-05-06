//
// Created by johnk on 15/1/2022.
//

#include <RHI/DirectX12/Common.h>
#include <RHI/DirectX12/Gpu.h>
#include <RHI/DirectX12/Device.h>
#include <RHI/DirectX12/Queue.h>
#include <RHI/DirectX12/Buffer.h>
#include <RHI/DirectX12/Texture.h>
#include <RHI/DirectX12/Sampler.h>
#include <RHI/DirectX12/BindGroupLayout.h>
#include <RHI/DirectX12/BindGroup.h>
#include <RHI/DirectX12/PipelineLayout.h>
#include <RHI/DirectX12/ShaderModule.h>
#include <RHI/DirectX12/Pipeline.h>
#include <RHI/DirectX12/CommandBuffer.h>
#include <RHI/DirectX12/SwapChain.h>

namespace RHI::DirectX12 {
    DX12Device::DX12Device(DX12Gpu& g, const DeviceCreateInfo* createInfo) : Device(createInfo), gpu(g), rtvDescriptorSize(0), cbvSrvUavDescriptorSize(0)
    {
        CreateDX12Device();
        CreateDX12Queues(createInfo);
        CreateDX12CommandAllocator();
        GetDX12DescriptorSize();
    }

    DX12Device::~DX12Device() = default;

    void DX12Device::Destroy()
    {
        delete this;
    }

    size_t DX12Device::GetQueueNum(QueueType type)
    {
        auto iter = queues.find(type);
        Assert(iter != queues.end());
        return iter->second.size();
    }

    Queue* DX12Device::GetQueue(QueueType type, size_t index)
    {
        auto iter = queues.find(type);
        Assert(iter != queues.end());
        auto& queueArray = iter->second;
        Assert(index >= 0 && index < queueArray.size());
        return queueArray[index].get();
    }

    SwapChain* DX12Device::CreateSwapChain(const SwapChainCreateInfo* createInfo)
    {
        return new DX12SwapChain(*this, createInfo);
    }

    ComPtr<ID3D12CommandAllocator>& DX12Device::GetDX12CommandAllocator()
    {
        return dx12CommandAllocator;
    }

    Buffer* DX12Device::CreateBuffer(const BufferCreateInfo* createInfo)
    {
        return new DX12Buffer(*this, createInfo);
    }

    Texture* DX12Device::CreateTexture(const TextureCreateInfo* createInfo)
    {
        return new DX12Texture(*this, createInfo);
    }

    Sampler* DX12Device::CreateSampler(const SamplerCreateInfo* createInfo)
    {
        return new DX12Sampler(*this, createInfo);
    }

    BindGroupLayout* DX12Device::CreateBindGroupLayout(const BindGroupLayoutCreateInfo* createInfo)
    {
        return new DX12BindGroupLayout(createInfo);
    }

    BindGroup* DX12Device::CreateBindGroup(const BindGroupCreateInfo* createInfo)
    {
        return new DX12BindGroup(createInfo);
    }

    PipelineLayout* DX12Device::CreatePipelineLayout(const PipelineLayoutCreateInfo* createInfo)
    {
        return new DX12PipelineLayout(*this, createInfo);
    }

    ShaderModule* DX12Device::CreateShaderModule(const ShaderModuleCreateInfo* createInfo)
    {
        return new DX12ShaderModule(createInfo);
    }

    ComputePipeline* DX12Device::CreateComputePipeline(const ComputePipelineCreateInfo* createInfo)
    {
        return new DX12ComputePipeline(*this, createInfo);
    }

    GraphicsPipeline* DX12Device::CreateGraphicsPipeline(const GraphicsPipelineCreateInfo* createInfo)
    {
        return new DX12GraphicsPipeline(*this, createInfo);
    }

    CommandBuffer* DX12Device::CreateCommandBuffer()
    {
        return new DX12CommandBuffer(*this);
    }

    DX12Gpu& DX12Device::GetGpu()
    {
        return gpu;
    }

    ComPtr<ID3D12Device>& DX12Device::GetDX12Device()
    {
        return dx12Device;
    }

    DescriptorAllocation DX12Device::AllocateRtvDescriptor()
    {
        return AllocateDescriptor(rtvHeapList, 4, rtvDescriptorSize, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
    }

    DescriptorAllocation DX12Device::AllocateCbvSrvUavDescriptor()
    {
        return AllocateDescriptor(cbvSrvUavHeapList, 4, cbvSrvUavDescriptorSize, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
    }

    DescriptorAllocation DX12Device::AllocateSamplerDescriptor()
    {
        return AllocateDescriptor(samplerHeapList, 4, samplerDescriptorSize, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
    }

    DescriptorAllocation DX12Device::AllocateDescriptor(std::list<DescriptorHeapListNode>& list, uint8_t capacity, uint32_t descriptorSize, D3D12_DESCRIPTOR_HEAP_TYPE heapType, D3D12_DESCRIPTOR_HEAP_FLAGS heapFlag)
    {
        if (list.empty() || list.back().used >= capacity) {
            DescriptorHeapListNode node {};

            D3D12_DESCRIPTOR_HEAP_DESC desc {};
            desc.NumDescriptors = capacity;
            desc.Type = heapType;
            desc.Flags = heapFlag;
            bool success = SUCCEEDED(dx12Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&node.descriptorHeap)));
            Assert(success);
            list.emplace_back(std::move(node));
        }

        auto& last = list.back();

        CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(last.descriptorHeap->GetCPUDescriptorHandleForHeapStart());
        CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(last.descriptorHeap->GetGPUDescriptorHandleForHeapStart());
        auto offset = last.used++;
        return {
            cpuHandle.Offset(offset, descriptorSize),
            gpuHandle.Offset(offset, descriptorSize),
            last.descriptorHeap.Get()
        };
    }

    void DX12Device::CreateDX12Device()
    {
        bool success = SUCCEEDED(D3D12CreateDevice(gpu.GetDX12Adapter().Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&dx12Device)));
        Assert(success);
    }

    void DX12Device::CreateDX12Queues(const DeviceCreateInfo* createInfo)
    {
        std::unordered_map<QueueType, size_t> queueNumMap;
        for (size_t i = 0; i < createInfo->queueCreateInfoNum; i++) {
            const auto& queueCreateInfo = createInfo->queueCreateInfos[i];
            auto iter = queueNumMap.find(queueCreateInfo.type);
            if (iter == queueNumMap.end()) {
                queueNumMap[queueCreateInfo.type] = 0;
            }
            queueNumMap[queueCreateInfo.type] += queueCreateInfo.num;
        }

        for (auto iter : queueNumMap) {
            std::vector<std::unique_ptr<DX12Queue>> tempQueues(iter.second);

            D3D12_COMMAND_QUEUE_DESC queueDesc {};
            queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
            queueDesc.Type = DX12EnumCast<QueueType, D3D12_COMMAND_LIST_TYPE>(iter.first);
            for (auto& j : tempQueues) {
                ComPtr<ID3D12CommandQueue> commandQueue;
                dx12Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue));
                Assert(commandQueue);
                j = std::make_unique<DX12Queue>(std::move(commandQueue));
            }

            queues[iter.first] = std::move(tempQueues);
        }
    }

    void DX12Device::CreateDX12CommandAllocator()
    {
        dx12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&dx12CommandAllocator));
    }

    void DX12Device::GetDX12DescriptorSize()
    {
        rtvDescriptorSize = dx12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        cbvSrvUavDescriptorSize = dx12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }
}
