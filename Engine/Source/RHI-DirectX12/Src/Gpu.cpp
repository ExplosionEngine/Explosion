//
// Created by johnk on 13/1/2022.
//

#include <RHI/DirectX12/Common.h>
#include <RHI/DirectX12/Gpu.h>
#include <RHI/DirectX12/Device.h>

namespace RHI::DirectX12 {
    DX12Gpu::DX12Gpu(DX12Instance& inInstance, ComPtr<IDXGIAdapter1>&& inNativeAdapter)
        : instance(inInstance)
        , nativeAdapter(inNativeAdapter)
    {
    }

    DX12Gpu::~DX12Gpu() = default;

    GpuProperty DX12Gpu::GetProperty()
    {
        DXGI_ADAPTER_DESC1 desc;
        Assert(SUCCEEDED(nativeAdapter->GetDesc1(&desc)));

        GpuProperty property {};
        property.vendorId = desc.VendorId;
        property.deviceId = desc.DeviceId;
        property.type = desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE ? GpuType::software : GpuType::hardware;
        return property;
    }

    DX12Instance& DX12Gpu::GetInstance() const
    {
        return instance;
    }

    IDXGIAdapter1* DX12Gpu::GetNative() const
    {
        return nativeAdapter.Get();
    }

    Common::UniquePtr<Device> DX12Gpu::RequestDevice(const DeviceCreateInfo& inCreateInfo)
    {
        return Common::UniquePtr<Device>(new DX12Device(*this, inCreateInfo));
    }
}
