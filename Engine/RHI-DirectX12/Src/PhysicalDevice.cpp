//
// Created by johnk on 30/12/2021.
//

#include <RHI/DirectX12/PhysicalDevice.h>

namespace RHI {
    DirectX12::DX12PhysicalDevice::DX12PhysicalDevice(ComPtr<IDXGIAdapter1>&& adapter) : adapter(adapter) {}

    DirectX12::DX12PhysicalDevice::~DX12PhysicalDevice() = default;
}
