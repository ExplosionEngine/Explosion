//
// Created by johnk on 15/1/2022.
//

#include <RHI/DirectX12/Common.h>
#include <RHI/DirectX12/Gpu.h>
#include <RHI/DirectX12/Device.h>
#include <RHI/DirectX12/Queue.h>

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
        for (size_t i = 0; i < createInfo->queueCreateInfoNum; i++) {
            auto& queueCreateInfo = createInfo->queueCreateInfos[i];
            std::vector<std::unique_ptr<DX12Queue>> temp(queueCreateInfo.num);

            D3D12_COMMAND_QUEUE_DESC queueDesc {};
            queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
            queueDesc.Type = DX12EnumCast<QueueType, D3D12_COMMAND_LIST_TYPE>(queueCreateInfo.type);
            for (auto& j : temp) {
                ComPtr<ID3D12CommandQueue> commandQueue;
                dx12Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue));
                j = std::make_unique<DX12Queue>(std::move(commandQueue));
            }

            queues[queueCreateInfo.type] = std::move(temp);
        }
    }
}
