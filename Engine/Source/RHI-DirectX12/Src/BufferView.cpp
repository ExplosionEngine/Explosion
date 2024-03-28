//
// Created by johnk on 20/3/2022.
//

#include <utility>

#include <RHI/DirectX12/Device.h>
#include <RHI/DirectX12/Buffer.h>
#include <RHI/DirectX12/BufferView.h>
#include <RHI/DirectX12/Common.h>

namespace RHI::DirectX12 {
    static inline bool IsConstantBuffer(BufferViewType type, BufferUsageFlags bufferUsages)
    {
        return type == BufferViewType::uniformBinding && (bufferUsages & BufferUsageBits::uniform) != 0;
    }

    static inline bool IsUnorderedAccessBuffer(BufferViewType type, BufferUsageFlags bufferUsages)
    {
        return type == BufferViewType::storageBinding && (bufferUsages & BufferUsageBits::storage) != 0;
    }

    static inline bool IsVertexBuffer(BufferViewType type, BufferUsageFlags bufferUsages)
    {
        return type == BufferViewType::vertex && (bufferUsages & BufferUsageBits::vertex) != 0;
    }

    static inline bool IsIndexBuffer(BufferViewType type, BufferUsageFlags bufferUsages)
    {
        return type == BufferViewType::index && (bufferUsages & BufferUsageBits::index) != 0;
    }
}

namespace RHI::DirectX12 {
    DX12BufferView::DX12BufferView(DX12Buffer& buffer, const BufferViewCreateInfo& createInfo)
        : BufferView(createInfo), buffer(buffer)
    {
        CreateDX12Descriptor(createInfo);
    }

    DX12BufferView::~DX12BufferView() = default;

    void DX12BufferView::Destroy()
    {
        delete this;
    }

    CD3DX12_CPU_DESCRIPTOR_HANDLE DX12BufferView::GetDX12CpuDescriptorHandle() const
    {
        return descriptor.dx12CpuDescriptorHandle;
    }

    const D3D12_VERTEX_BUFFER_VIEW& DX12BufferView::GetDX12VertexBufferView() const
    {
        return vertex.dx12VertexBufferView;
    }

    const D3D12_INDEX_BUFFER_VIEW& DX12BufferView::GetDX12IndexBufferView() const
    {
        return index.dx12IndexBufferView;
    }

    void DX12BufferView::CreateDX12Descriptor(const BufferViewCreateInfo& createInfo)
    {
        if (IsConstantBuffer(createInfo.type, buffer.GetUsages())) {
            D3D12_CONSTANT_BUFFER_VIEW_DESC desc {};
            desc.BufferLocation = buffer.GetDX12Resource()->GetGPUVirtualAddress() + createInfo.offset;
            desc.SizeInBytes = GetConstantBufferSize(createInfo.size);

            auto allocation = buffer.GetDevice().AllocateCbvSrvUavDescriptor();
            descriptor.dx12CpuDescriptorHandle = allocation.cpuHandle;
            buffer.GetDevice().GetDX12Device()->CreateConstantBufferView(&desc, descriptor.dx12CpuDescriptorHandle);
        } else if (IsUnorderedAccessBuffer(createInfo.type, buffer.GetUsages())) {
            D3D12_UNORDERED_ACCESS_VIEW_DESC desc {};
            desc.Format = DXGI_FORMAT_UNKNOWN;
            desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
            desc.Buffer.FirstElement = createInfo.offset;
            desc.Buffer.NumElements = createInfo.size;

            auto allocation = buffer.GetDevice().AllocateCbvSrvUavDescriptor();
            descriptor.dx12CpuDescriptorHandle = allocation.cpuHandle;
            buffer.GetDevice().GetDX12Device()->CreateUnorderedAccessView(buffer.GetDX12Resource().Get(), nullptr, &desc, descriptor.dx12CpuDescriptorHandle);
        } else if (IsVertexBuffer(createInfo.type, buffer.GetUsages())) {
            vertex.dx12VertexBufferView.BufferLocation = buffer.GetDX12Resource()->GetGPUVirtualAddress() + createInfo.offset;
            vertex.dx12VertexBufferView.SizeInBytes = createInfo.size;
            vertex.dx12VertexBufferView.StrideInBytes = std::get<VertexBufferViewInfo>(createInfo.extend).stride;
        } else if (IsIndexBuffer(createInfo.type, buffer.GetUsages())) {
            index.dx12IndexBufferView.BufferLocation = buffer.GetDX12Resource()->GetGPUVirtualAddress() + createInfo.offset;
            index.dx12IndexBufferView.SizeInBytes = createInfo.size;
            index.dx12IndexBufferView.Format = DX12EnumCast<IndexFormat, DXGI_FORMAT>(std::get<IndexBufferViewInfo>(createInfo.extend).format);
        }
    }
}
