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
        NonCopyable(DX12Gpu)
        DX12Gpu(DX12Instance& inInstance, ComPtr<IDXGIAdapter1>&& inNativeAdapter);
        ~DX12Gpu() override;

        GpuProperty GetProperty() override;
        Device* RequestDevice(const DeviceCreateInfo& inCreateInfo) override;

        DX12Instance& GetInstance();
        IDXGIAdapter1* GetNative();

    private:
        DX12Instance& instance;
        ComPtr<IDXGIAdapter1> nativeAdapter;
    };
}
