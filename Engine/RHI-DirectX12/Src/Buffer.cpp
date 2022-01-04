//
// Created by johnk on 4/1/2022.
//

#include <RHI/DirectX12/Buffer.h>

namespace RHI::DirectX12 {
    DX12Buffer::DX12Buffer(const BufferCreateInfo* createInfo)
        : Buffer(createInfo), deviceMemory(nullptr), size(createInfo->size), usage(createInfo->usage) {}

    DX12Buffer::~DX12Buffer() = default;

    void DX12Buffer::BindMemory(DX12DeviceMemory* dm)
    {
        deviceMemory = dm;
    }

    DX12DeviceMemory* DX12Buffer::GetDeviceMemory()
    {
        return deviceMemory;
    }
}
