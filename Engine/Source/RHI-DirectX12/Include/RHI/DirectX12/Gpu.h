//
// Created by johnk on 13/1/2022.
//

#pragma once

#include <wrl/client.h>
#include <dxgi1_4.h>

#include <RHI/Gpu.h>

using Microsoft::WRL::ComPtr;

namespace RHI::DirectX12 {
    class DX12Instance;

    class DX12Gpu : public Gpu {
    public:
        NON_COPYABLE(DX12Gpu)
        DX12Gpu(DX12Instance& instance, ComPtr<IDXGIAdapter1>&& adapter);
        ~DX12Gpu() override;

        GpuProperty GetProperty() override;
        Device* RequestDevice(const DeviceCreateInfo& createInfo) override;

        DX12Instance& GetInstance();
        ComPtr<IDXGIAdapter1>& GetDX12Adapter();

    private:
        DX12Instance& instance;
        ComPtr<IDXGIAdapter1> dx12Adapter;
    };
}
