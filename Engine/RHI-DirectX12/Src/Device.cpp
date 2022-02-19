//
// Created by johnk on 15/1/2022.
//

#include <RHI/DirectX12/Common.h>
#include <RHI/DirectX12/Gpu.h>
#include <RHI/DirectX12/Device.h>
#include <RHI/DirectX12/Queue.h>
#include <RHI/DirectX12/Buffer.h>

namespace RHI::DirectX12 {
    DX12Device::DX12Device(DX12Gpu& gpu, const DeviceCreateInfo* createInfo) : Device(createInfo)
    {
        CreateDevice(gpu);
        CreateQueues(createInfo);
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

    ComPtr<ID3D12Device>& DX12Device::GetDX12Device()
    {
        return dx12Device;
    }

    void DX12Device::CreateDevice(DX12Gpu& gpu)
    {
        if (FAILED(D3D12CreateDevice(gpu.GetDX12Adapter().Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&dx12Device)))) {
            throw DX12Exception("failed to create dx12 device");
        }
    }

    void DX12Device::CreateQueues(const DeviceCreateInfo* createInfo)
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
}
