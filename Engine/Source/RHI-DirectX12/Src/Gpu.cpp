//
// Created by johnk on 13/1/2022.
//

#include <RHI/DirectX12/Common.h>
#include <RHI/DirectX12/Gpu.h>
#include <RHI/DirectX12/Device.h>

namespace RHI::DirectX12 {
    DX12Gpu::DX12Gpu(DX12Instance& i, ComPtr<IDXGIAdapter1>&& a) : Gpu(), instance(i), dx12Adapter(a) {}

    DX12Gpu::~DX12Gpu() = default;

    GpuProperty DX12Gpu::GetProperty()
    {
        DXGI_ADAPTER_DESC1 desc;
        dx12Adapter->GetDesc1(&desc);

        GpuProperty property {};
        property.vendorId = desc.VendorId;
        property.deviceId = desc.DeviceId;
        property.type = GetGpuTypeByAdapterFlag(desc.Flags);
        return property;
    }

    DX12Instance& DX12Gpu::GetInstance()
    {
        return instance;
    }

    ComPtr<IDXGIAdapter1>& DX12Gpu::GetDX12Adapter()
    {
        return dx12Adapter;
    }

    Device* DX12Gpu::RequestDevice(const DeviceCreateInfo* createInfo)
    {
        return new DX12Device(*this, createInfo);
    }
}
