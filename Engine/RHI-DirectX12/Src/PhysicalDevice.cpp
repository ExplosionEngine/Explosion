//
// Created by johnk on 30/12/2021.
//

#include <RHI/DirectX12/PhysicalDevice.h>

namespace RHI::DirectX12 {
    DX12PhysicalDevice::DX12PhysicalDevice(ComPtr<IDXGIAdapter1>&& adapter) : dxgiAdapter(adapter) {}

    DX12PhysicalDevice::~DX12PhysicalDevice() = default;

    PhysicalDeviceProperty DX12PhysicalDevice::GetProperty()
    {
        DXGI_ADAPTER_DESC1 desc;
        dxgiAdapter->GetDesc1(&desc);

        PhysicalDeviceProperty property {};
        property.isSoftware = desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE;
        return property;
    }
}
