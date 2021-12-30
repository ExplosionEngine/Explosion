//
// Created by johnk on 31/12/2021.
//

#include <RHI/DirectX12/Instance.h>
#include <RHI/DirectX12/PhysicalDevice.h>
#include <RHI/DirectX12/LogicalDevice.h>
#include <RHI/DirectX12/Utility.h>

namespace RHI::DirectX12 {
    DX12LogicalDevice::DX12LogicalDevice(DX12Instance& instance, DX12PhysicalDevice& physicalDevice)
    {
        CreateDevice(instance.GetDXGIFactory(), physicalDevice.GetDXGIAdapter());
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
}
