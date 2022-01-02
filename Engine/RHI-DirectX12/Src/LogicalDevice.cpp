//
// Created by johnk on 31/12/2021.
//

#include <RHI/DirectX12/Instance.h>
#include <RHI/DirectX12/PhysicalDevice.h>
#include <RHI/DirectX12/LogicalDevice.h>
#include <RHI/DirectX12/Queue.h>
#include <RHI/DirectX12/Utility.h>

namespace RHI::DirectX12 {
    DX12LogicalDevice::DX12LogicalDevice(DX12Instance& instance, DX12PhysicalDevice& physicalDevice, const LogicalDeviceCreateInfo* createInfo)
        : LogicalDevice(createInfo), instance(instance)
    {
        CreateDevice(instance.GetDXGIFactory(), physicalDevice.GetDXGIAdapter());
        CreateCommandQueue(createInfo);
    }

    DX12LogicalDevice::~DX12LogicalDevice() = default;

    void DX12LogicalDevice::CreateDevice(ComPtr<IDXGIFactory4>& dxgiFactory, ComPtr<IDXGIAdapter1>& dxgiAdapter)
    {
        ThrowIfFailed(
            D3D12CreateDevice(dxgiAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&dx12Device)),
            "failed to create dx12 logical device"
        );
    }

    ComPtr<ID3D12Device>& DX12LogicalDevice::GetDX12Device()
    {
        return dx12Device;
    }

    void DX12LogicalDevice::CreateCommandQueue(const LogicalDeviceCreateInfo* createInfo)
    {
        for (size_t i = 0; i < createInfo->queueFamilyNum; i++) {
            auto& familyCreateInfo = createInfo->queueFamilyCreateInfos[i];
            auto& familyQueues = queueFamilies[familyCreateInfo.type];
            familyQueues.resize(familyCreateInfo.queueNum);

            QueueCreateInfo commandQueueCreateInfo {};
            commandQueueCreateInfo.type = familyCreateInfo.type;
            for (auto& familyQueue : familyQueues) {
                familyQueue = std::make_unique<DX12Queue>(*this, &commandQueueCreateInfo);
            }
        }
    }

    size_t DX12LogicalDevice::GetQueueNum(QueueFamilyType familyType)
    {
        auto iter = queueFamilies.find(familyType);
        if (iter == queueFamilies.end()) {
            return 0;
        }
        return iter->second.size();
    }

    Queue* DX12LogicalDevice::GetCommandQueue(QueueFamilyType familyType, size_t idx)
    {
        auto iter = queueFamilies.find(familyType);
        if (iter == queueFamilies.end()) {
            return nullptr;
        }
        if (idx < 0 || idx >= iter->second.size()) {
            return nullptr;
        }
        return iter->second[idx].get();
    }
}
