//
// Created by johnk on 27/12/2021.
//

#include <cstdint>
#include <unordered_map>

#include <RHI/Enum.h>
#include <RHI/DirectX12/Instance.h>
#include <RHI/DirectX12/PhysicalDevice.h>
#include <RHI/DirectX12/LogicalDevice.h>
#include <RHI/DirectX12/Surface.h>
#include <RHI/DirectX12/Utility.h>

extern "C" {
    RHI::Instance* RHICreateInstance(const RHI::InstanceCreateInfo* info)
    {
        return new RHI::DirectX12::DX12Instance(info);
    }
}

namespace RHI::DirectX12 {
    DX12Instance::DX12Instance(const InstanceCreateInfo* info) : Instance(info), property({})
    {
        ProduceExtensions(info);
        CreateFactory(info);
        LoadPhysicalDevices();
    }

    DX12Instance::~DX12Instance() = default;

    void DX12Instance::CreateFactory(const InstanceCreateInfo* info)
    {
        uint32_t dxgiFactoryFlags = 0;
        if (info->debugMode) {
            ComPtr<ID3D12Debug> debugController;
            ThrowIfFailed(
                D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)),
                "failed to get debug interface"
            );
            debugController->EnableDebugLayer();
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }

        ThrowIfFailed(
            CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&dxgiFactory)),
            "failed to create DXGI factory"
        );
    }

    void DX12Instance::LoadPhysicalDevices()
    {
        ComPtr<IDXGIAdapter1> adapter;
        while (SUCCEEDED(dxgiFactory->EnumAdapters1(physicalDevices.size(), &adapter))) {
            physicalDevices.emplace_back(std::make_unique<DX12PhysicalDevice>(std::move(adapter)));
            adapter = nullptr;
        }
    }

    uint32_t DX12Instance::CountPhysicalDevices()
    {
        return physicalDevices.size();
    }

    PhysicalDevice* DX12Instance::GetPhysicalDevice(uint32_t idx)
    {
        return physicalDevices[idx].get();
    }

    LogicalDevice* DX12Instance::CreateLogicalDevice(PhysicalDevice* physicalDevice)
    {
        return new DX12LogicalDevice(*this, *static_cast<DX12PhysicalDevice*>(physicalDevice));
    }

    void DX12Instance::DestroyLogicalDevice(LogicalDevice* logicalDevice)
    {
        delete logicalDevice;
    }

    Surface* DX12Instance::CreateSurface(const SurfaceCreateInfo* createInfo)
    {
        return new DX12Surface(*this, createInfo);
    }

    void DX12Instance::DestroySurface(Surface* surface)
    {
        delete surface;
    }

    const DX12InstanceProperty& DX12Instance::GetProperty()
    {
        return property;
    }

    ComPtr<IDXGIFactory4>& DX12Instance::GetDXGIFactory()
    {
        return dxgiFactory;
    }

    void DX12Instance::ProduceExtensions(const InstanceCreateInfo* info)
    {
        static const std::unordered_map<std::string, ExtensionProducer> EXTENSION_PRODUCERS = {
            { RHI_EXT_NAME_SURFACE, [](DX12InstanceProperty& prop) -> void { prop.supportSurface = true; } },
            { RHI_EXT_NAME_WINDOWS_SURFACE, [](DX12InstanceProperty& prop) -> void { prop.supportWindowsSurface = true; } }
        };

        for (size_t i = 0; i < info->extensionNum; i++) {
            auto iter = EXTENSION_PRODUCERS.find(std::string(info->extensions[i]));
            if (iter == EXTENSION_PRODUCERS.end()) {
                continue;
            }
            iter->second(property);
        }
    }
}
