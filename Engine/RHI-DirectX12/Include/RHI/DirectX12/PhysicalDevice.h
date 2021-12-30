//
// Created by johnk on 30/12/2021.
//

#ifndef EXPLOSION_RHI_DX12_PHYSICAL_DEVICE_H
#define EXPLOSION_RHI_DX12_PHYSICAL_DEVICE_H

#include <cstdint>

#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>

#include <RHI/PhysicalDevice.h>

using namespace Microsoft::WRL;

namespace RHI::DirectX12 {
    class DX12PhysicalDevice : public PhysicalDevice {
    public:
        NON_COPYABLE(DX12PhysicalDevice)
        DX12PhysicalDevice(ComPtr<IDXGIAdapter1>&& adapter);
        ~DX12PhysicalDevice() override;

    private:
        ComPtr<IDXGIAdapter1> adapter;
    };
}

#endif //EXPLOSION_RHI_DX12_PHYSICAL_DEVICE_H
