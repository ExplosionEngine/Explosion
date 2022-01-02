//
// Created by johnk on 27/12/2021.
//

#ifndef EXPLOSION_RHI_DX12_INSTANCE_H
#define EXPLOSION_RHI_DX12_INSTANCE_H

#include <vector>
#include <memory>
#include <functional>

#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>

#include <RHI/Instance.h>
#include <RHI/DirectX12/Api.h>
#include <Common/Utility.h>

using namespace Microsoft::WRL;

extern "C" {
    RHI_DIRECTX12_API RHI::Instance* RHICreateInstance(const RHI::InstanceCreateInfo* info);
};

namespace RHI::DirectX12 {
    class DX12PhysicalDevice;

    struct DX12InstanceProperty {
        bool supportSurface;
        bool supportWindowsSurface;
    };

    using InstanceExtProcessor = std::function<void(DX12InstanceProperty&)>;

    class DX12Instance : public Instance {
    public:
        NON_COPYABLE(DX12Instance)
        explicit DX12Instance(const InstanceCreateInfo* info);
        ~DX12Instance() override;

        uint32_t CountPhysicalDevices() override;
        PhysicalDevice* GetPhysicalDevice(uint32_t idx) override;
        LogicalDevice* CreateLogicalDevice(PhysicalDevice* physicalDevice, const LogicalDeviceCreateInfo* createInfo) override;
        void DestroyLogicalDevice(LogicalDevice* logicalDevice) override;
        Surface* CreateSurface(const SurfaceCreateInfo* createInfo) override;
        void DestroySurface(Surface* surface) override;

        const DX12InstanceProperty& GetProperty();
        ComPtr<IDXGIFactory4>& GetDXGIFactory();

    private:
        void CreateFactory(const InstanceCreateInfo* info);
        void ProcessExtensions(const InstanceCreateInfo* info);
        void LoadPhysicalDevices();

        ComPtr<IDXGIFactory4> dxgiFactory;

        DX12InstanceProperty property;
        std::vector<std::unique_ptr<DX12PhysicalDevice>> physicalDevices;
    };
}

#endif //EXPLOSION_RHI_DX12_INSTANCE_H
