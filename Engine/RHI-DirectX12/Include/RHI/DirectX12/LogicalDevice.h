//
// Created by johnk on 31/12/2021.
//

#ifndef EXPLOSION_RHI_DX12_LOGICAL_DEVICE_H
#define EXPLOSION_RHI_DX12_LOGICAL_DEVICE_H

#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>

#include <RHI/LogicalDevice.h>

using namespace Microsoft::WRL;

namespace RHI::DirectX12 {
    class DX12Instance;
    class DX12PhysicalDevice;

    class DX12LogicalDevice : public LogicalDevice {
    public:
        NON_COPYABLE(DX12LogicalDevice)
        DX12LogicalDevice(DX12Instance& instance, DX12PhysicalDevice& physicalDevice);
        ~DX12LogicalDevice() override;

        CommandQueue* CreateCommandQueue(const CommandQueueCreateInfo& createInfo) override;
        void DestroyCommandQueue(CommandQueue* commandQueue) override;

        ComPtr<ID3D12Device>& GetDX12Device();

    private:
        void CreateDevice(ComPtr<IDXGIFactory4>& dxgiFactory, ComPtr<IDXGIAdapter1>& dxgiAdapter);

        ComPtr<ID3D12Device> dx12Device;
    };
}

#endif //EXPLOSION_RHI_DX12_LOGICAL_DEVICE_H
