//
// Created by johnk on 15/1/2022.
//

#include <RHI/DirectX12/Common.h>
#include <RHI/DirectX12/Gpu.h>
#include <RHI/DirectX12/Device.h>
#include <RHI/DirectX12/Queue.h>
#include <RHI/DirectX12/Buffer.h>

namespace RHI::DirectX12 {
    DX12Device::DX12Device(DX12Gpu& gpu, const DeviceCreateInfo* createInfo) : Device(createInfo), rtvDescriptorSize(0), cbvSrvUavDescriptorSize(0)
    {
        CreateDX12Device(gpu);
        CreateDX12Queues(createInfo);
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
        if (iter == queues.end()) {
            throw DX12Exception("failed to find specific queue family");
        }
        return iter->second.size();
    }

    Queue* DX12Device::GetQueue(QueueType type, size_t index)
    {
        auto iter = queues.find(type);
        if (iter == queues.end()) {
            throw DX12Exception("failed to find specific queue family");
        }
        auto& queueArray = iter->second;
        if (index < 0 || index >= queueArray.size()) {
            throw DX12Exception("bad queue index");
        }
        return queueArray[index].get();
    }

    Buffer* DX12Device::CreateBuffer(const BufferCreateInfo* createInfo)
    {
        return new DX12Buffer(*this, createInfo);
    }

    Texture* DX12Device::CreateTexture(const TextureCreateInfo* createInfo)
    {
        // TODO
        return nullptr;
    }

    Sampler* DX12Device::CreateSampler(const SamplerCreateInfo* createInfo)
    {
        // TODO
        return nullptr;
    }

    BindGroupLayout* DX12Device::CreateBindGroupLayout(const BindGroupLayoutCreateInfo* createInfo)
    {
        // TODO
        return nullptr;
    }

    BindGroup* DX12Device::CreateBindGroup(const BindGroupCreateInfo* createInfo)
    {
        // TODO
        return nullptr;
    }

    PipelineLayout* DX12Device::CreatePipelineLayout(const PipelineLayoutCreateInfo* createInfo)
    {
        // TODO
        return nullptr;
    }

    ShaderModule* DX12Device::CreateShaderModule(const ShaderModuleCreateInfo* createInfo)
    {
        // TODO
        return nullptr;
    }

    ComputePipeline* DX12Device::CreateComputePipeline(const ComputePipelineCreateInfo* createInfo)
    {
        // TODO
        return nullptr;
    }

    GraphicsPipeline* DX12Device::CreateGraphicsPipeline(const GraphicsPipelineCreateInfo* createInfo)
    {
        // TODO
        return nullptr;
    }

    CommandBuffer* DX12Device::CreateCommandBuffer(const CommandBufferCreateInfo* createInfo)
    {
        // TODO
        return nullptr;
    }

    ComPtr<ID3D12Device>& DX12Device::GetDX12Device()
    {
        return dx12Device;
    }

    CD3DX12_CPU_DESCRIPTOR_HANDLE DX12Device::AllocateRtvDescriptor()
    {
        return AllocateDescriptor(rtvHeapList, 4, rtvDescriptorSize, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
    }

    CD3DX12_CPU_DESCRIPTOR_HANDLE DX12Device::AllocateCbvSrvUavDescriptor()
    {
        return AllocateDescriptor(cbvSrvUavHeapList, 4, cbvSrvUavDescriptorSize, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
    }

    CD3DX12_CPU_DESCRIPTOR_HANDLE DX12Device::AllocateSamplerDescriptor()
    {
        return AllocateDescriptor(samplerHeapList, 4, samplerDescriptorSize, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
    }

    std::vector<ID3D12DescriptorHeap*> DX12Device::GetAllRtvDescriptorHeap()
    {
        return GetAllDescriptorHeap(rtvHeapList);
    }

    std::vector<ID3D12DescriptorHeap*> DX12Device::GetAllCbvSrvUavDescriptorHeap()
    {
        return GetAllDescriptorHeap(cbvSrvUavHeapList);
    }

    std::vector<ID3D12DescriptorHeap*> DX12Device::GetAllSamplerDescriptorHeap()
    {
        return GetAllDescriptorHeap(samplerHeapList);
    }

    std::vector<ID3D12DescriptorHeap*> DX12Device::GetAllDescriptorHeap(std::list<DescriptorHeapListNode>& list)
    {
        std::vector<ID3D12DescriptorHeap*> result;
        for (auto& iter : list) {
            result.emplace_back(iter.descriptorHeap.Get());
        }
        return result;
    }

    CD3DX12_CPU_DESCRIPTOR_HANDLE DX12Device::AllocateDescriptor(std::list<DescriptorHeapListNode>& list, uint8_t capacity, uint32_t descriptorSize, D3D12_DESCRIPTOR_HEAP_TYPE heapType, D3D12_DESCRIPTOR_HEAP_FLAGS heapFlag)
    {
        if (list.empty() || list.back().used >= capacity) {
            DescriptorHeapListNode node {};

            D3D12_DESCRIPTOR_HEAP_DESC desc {};
            desc.NumDescriptors = capacity;
            desc.Type = heapType;
            desc.Flags = heapFlag;
            if (FAILED(dx12Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&node.descriptorHeap)))) {
                throw DX12Exception("failed allocate new descriptor heap");
            }

            list.emplace_back(std::move(node));
        }

        auto& last = list.back();

        CD3DX12_CPU_DESCRIPTOR_HANDLE handle(last.descriptorHeap->GetCPUDescriptorHandleForHeapStart());
        return handle.Offset(last.used++, descriptorSize);
    }

    void DX12Device::CreateDX12Device(DX12Gpu& gpu)
    {
        if (FAILED(D3D12CreateDevice(gpu.GetDX12Adapter().Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&dx12Device)))) {
            throw DX12Exception("failed to create dx12 device");
        }
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
                if (commandQueue == nullptr) {
                    throw DX12Exception("failed to create dx12 queue with specific type");
                }
                j = std::make_unique<DX12Queue>(std::move(commandQueue));
            }

            queues[iter.first] = std::move(tempQueues);
        }
    }

    void DX12Device::GetDX12DescriptorSize()
    {
        rtvDescriptorSize = dx12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        cbvSrvUavDescriptorSize = dx12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }
}
