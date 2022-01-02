//
// Created by johnk on 31/12/2021.
//

#ifndef EXPLOSION_RHI_DX12_LOGICAL_DEVICE_H
#define EXPLOSION_RHI_DX12_LOGICAL_DEVICE_H

#include <unordered_map>
#include <memory>

#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>

#include <RHI/LogicalDevice.h>

using namespace Microsoft::WRL;

namespace RHI::DirectX12 {
    class DX12Instance;
    class DX12PhysicalDevice;

    class DX12LogicalDevice : public LogicalDevice {
    public:
        NON_COPYABLE(DX12LogicalDevice)
        DX12LogicalDevice(DX12Instance& instance, DX12PhysicalDevice& physicalDevice, const LogicalDeviceCreateInfo* createInfo);
        ~DX12LogicalDevice() override;

        size_t GetQueueNum(QueueFamilyType familyType) override;
        Queue* GetCommandQueue(QueueFamilyType familyType, size_t idx) override;

        ComPtr<ID3D12Device>& GetDX12Device();

    private:
        void CreateDevice(ComPtr<IDXGIFactory4>& dxgiFactory, ComPtr<IDXGIAdapter1>& dxgiAdapter);
        void CreateCommandQueue(const LogicalDeviceCreateInfo* createInfo);

        DX12Instance& instance;
        ComPtr<ID3D12Device> dx12Device;
        std::unordered_map<QueueFamilyType, std::vector<std::unique_ptr<Queue>>> queueFamilies;
    };
}

#endif //EXPLOSION_RHI_DX12_LOGICAL_DEVICE_H
