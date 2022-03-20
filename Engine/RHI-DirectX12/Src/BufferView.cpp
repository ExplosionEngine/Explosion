//
// Created by johnk on 20/3/2022.
//

#include <RHI/DirectX12/Device.h>
#include <RHI/DirectX12/Buffer.h>
#include <RHI/DirectX12/BufferView.h>

namespace RHI::DirectX12 {
    static inline bool IsConstantBuffer(BufferUsageFlags bufferUsages)
    {
        return bufferUsages & BufferUsageBits::UNIFORM;
    }

    static inline bool IsUnorderedAccessBuffer (BufferUsageFlags bufferUsages)
    {
        return bufferUsages & BufferUsageBits::STORAGE;
    }
}

namespace RHI::DirectX12 {
    DX12BufferView::DX12BufferView(DX12Buffer& buffer, const BufferViewCreateInfo* createInfo) : BufferView(createInfo), buffer(buffer)
    {
        CreateDX12Descriptor(createInfo);
    }

    DX12BufferView::~DX12BufferView() = default;

    void DX12BufferView::Destroy()
    {
        delete this;
    }

    CD3DX12_CPU_DESCRIPTOR_HANDLE DX12BufferView::GetDX12CpuDescriptorHandle()
    {
        return dx12CpuDescriptorHandle;
    }

    void DX12BufferView::CreateDX12Descriptor(const BufferViewCreateInfo* createInfo)
    {
        if (IsConstantBuffer(buffer.GetUsages())) {
            D3D12_CONSTANT_BUFFER_VIEW_DESC desc {};
            desc.BufferLocation = buffer.GetDX12Resource()->GetGPUVirtualAddress() + createInfo->offset;
            desc.SizeInBytes = createInfo->size;

            dx12CpuDescriptorHandle = buffer.GetDevice().AllocateCbvSrvUavDescriptor();
            buffer.GetDevice().GetDX12Device()->CreateConstantBufferView(&desc, dx12CpuDescriptorHandle);
        } else if (IsUnorderedAccessBuffer(buffer.GetUsages())) {
            D3D12_UNORDERED_ACCESS_VIEW_DESC desc {};
            desc.Format = DXGI_FORMAT_UNKNOWN;
            desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
            desc.Buffer.FirstElement = createInfo->offset;
            desc.Buffer.NumElements = createInfo->size;

            dx12CpuDescriptorHandle = buffer.GetDevice().AllocateCbvSrvUavDescriptor();
            buffer.GetDevice().GetDX12Device()->CreateUnorderedAccessView(buffer.GetDX12Resource().Get(), nullptr, &desc, dx12CpuDescriptorHandle);
        }
    }
}
