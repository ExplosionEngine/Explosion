//
// Created by johnk on 31/12/2021.
//

#ifndef EXPLOSION_RHI_DX12_LOGICAL_DEVICE_H
#define EXPLOSION_RHI_DX12_LOGICAL_DEVICE_H

#include <unordered_map>
#include <memory>
#include <functional>

#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>

#include <RHI/LogicalDevice.h>

using namespace Microsoft::WRL;

namespace RHI::DirectX12 {
    class DX12Instance;
    class DX12PhysicalDevice;

    struct DX12LogicalDeviceProperty {
        bool supportSwapChain;
    };

    using DeviceExtProcessor = std::function<void(DX12LogicalDeviceProperty&)>;

    class DX12LogicalDevice : public LogicalDevice {
    public:
        NON_COPYABLE(DX12LogicalDevice)
        DX12LogicalDevice(DX12Instance& instance, DX12PhysicalDevice& physicalDevice, const LogicalDeviceCreateInfo* createInfo);
        ~DX12LogicalDevice() override;

        size_t GetQueueNum(QueueFamilyType familyType) override;
        Queue* GetCommandQueue(QueueFamilyType familyType, size_t idx) override;
        SwapChain* CreateSwapChain(const SwapChainCreateInfo* createInfo) override;
        void DestroySwapChain(SwapChain* swapChain) override;
        DeviceMemory* AllocateDeviceMemory(const DeviceMemoryAllocateInfo* createInfo) override;
        void FreeDeviceMemory(DeviceMemory* deviceMemory) override;
        Buffer* CreateBuffer(const BufferCreateInfo* createInfo) override;
        void DestroyBuffer(Buffer* buffer) override;
        void BindBufferMemory(Buffer* buffer, DeviceMemory* deviceMemory) override;
        void* MapDeviceMemory(DeviceMemory* deviceMemory) override;
        void UnmapDeviceMemory(DeviceMemory* deviceMemory) override;

        DX12LogicalDeviceProperty GetProperty();
        ComPtr<ID3D12Device>& GetDX12Device();

    private:
        void CreateDevice(ComPtr<IDXGIFactory4>& dxgiFactory, ComPtr<IDXGIAdapter1>& dxgiAdapter);
        void ProcessExtensions(const LogicalDeviceCreateInfo* createInfo);
        void CreateCommandQueue(const LogicalDeviceCreateInfo* createInfo);

        ComPtr<ID3D12Device> dx12Device;

        DX12Instance& instance;
        DX12LogicalDeviceProperty property;
        std::unordered_map<QueueFamilyType, std::vector<std::unique_ptr<Queue>>> queueFamilies;
    };
}

#endif //EXPLOSION_RHI_DX12_LOGICAL_DEVICE_H
