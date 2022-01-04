//
// Created by johnk on 4/1/2022.
//

#ifndef EXPLOSION_RHI_DX12_DEVICE_MEMORY_H
#define EXPLOSION_RHI_DX12_DEVICE_MEMORY_H

#include <d3d12.h>
#include <wrl/client.h>

#include <RHI/DeviceMemory.h>

using namespace Microsoft::WRL;

namespace RHI::DirectX12 {
    class DX12LogicalDevice;

    class DX12DeviceMemory : public DeviceMemory {
    public:
        NON_COPYABLE(DX12DeviceMemory)
        explicit DX12DeviceMemory(DX12LogicalDevice& logicalDevice, const DeviceMemoryAllocateInfo* allocateInfo);
        ~DX12DeviceMemory() override;

        ComPtr<ID3D12Resource>& GetDX12Resource();

    private:
        void CreateResource(DX12LogicalDevice& logicalDevice, const DeviceMemoryAllocateInfo* allocateInfo);

        ComPtr<ID3D12Resource> dx12Resource;
    };
}

#endif //EXPLOSION_RHI_DX12_DEVICE_MEMORY_H
