//
// Created by johnk on 4/1/2022.
//

#include <directx/d3dx12.h>

#include <RHI/DirectX12/DeviceMemory.h>
#include <RHI/DirectX12/LogicalDevice.h>
#include <RHI/DirectX12/Enum.h>

namespace RHI::DirectX12 {
    DX12DeviceMemory::DX12DeviceMemory(ComPtr<ID3D12Resource>&& resource) : DeviceMemory(), dx12Resource(resource) {}

    DX12DeviceMemory::DX12DeviceMemory(DX12LogicalDevice& logicalDevice, const DeviceMemoryAllocateInfo* allocateInfo) : DeviceMemory(allocateInfo)
    {
        CreateResource(logicalDevice, allocateInfo);
    }

    DX12DeviceMemory::~DX12DeviceMemory() = default;

    ComPtr<ID3D12Resource>& DX12DeviceMemory::GetDX12Resource()
    {
        return dx12Resource;
    }

    void DX12DeviceMemory::CreateResource(DX12LogicalDevice& logicalDevice, const DeviceMemoryAllocateInfo* allocateInfo)
    {
        auto headType = EnumCast<DeviceMemoryType, D3D12_HEAP_TYPE>(allocateInfo->type);
        auto heapProperties = CD3DX12_HEAP_PROPERTIES(headType);
        auto desc = CD3DX12_RESOURCE_DESC::Buffer(allocateInfo->size);

        ThrowIfFailed(
            logicalDevice.GetDX12Device()->CreateCommittedResource(
                &heapProperties,
                D3D12_HEAP_FLAG_NONE,
                &desc,
                headType == D3D12_HEAP_TYPE_UPLOAD ? D3D12_RESOURCE_STATE_GENERIC_READ : D3D12_RESOURCE_STATE_COPY_DEST,
                nullptr,
                IID_PPV_ARGS(&dx12Resource)
            ),
            "failed to create dx12 resource"
        );
    }
}
