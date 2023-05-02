//
// Created by johnk on 20/3/2022.
//

#include <RHI/DirectX12/Device.h>
#include <RHI/DirectX12/Buffer.h>
#include <RHI/DirectX12/BufferView.h>
#include <RHI/DirectX12/Common.h>

namespace RHI::DirectX12 {
    static inline bool IsConstantBuffer(BufferUsageFlags bufferUsages)
    {
        return (bufferUsages & BufferUsageBits::UNIFORM) != 0;
    }

    static inline bool IsUnorderedAccessBuffer(BufferUsageFlags bufferUsages)
    {
        return (bufferUsages & BufferUsageBits::STORAGE) != 0;
    }

    static inline bool IsVertexBuffer(BufferUsageFlags bufferUsages)
    {
        return (bufferUsages & BufferUsageBits::VERTEX) != 0;
    }

    static inline bool IsIndexBuffer(BufferUsageFlags bufferUsages)
    {
        return (bufferUsages & BufferUsageBits::INDEX) != 0;
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

    CD3DX12_GPU_DESCRIPTOR_HANDLE DX12BufferView::GetDX12GpuDescriptorHandle() const
    {
        return descriptor.dx12GpuDescriptorHandle;
    }

    ID3D12DescriptorHeap* DX12BufferView::GetDX12DescriptorHeap() const
    {
        return descriptor.dx12DescriptorHeap;
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
        if (IsConstantBuffer(buffer.GetUsages())) {
            D3D12_CONSTANT_BUFFER_VIEW_DESC desc {};
            desc.BufferLocation = buffer.GetDX12Resource()->GetGPUVirtualAddress() + createInfo.offset;
            desc.SizeInBytes = GetConstantBufferSize(createInfo.size);

            auto allocation = buffer.GetDevice().AllocateCbvSrvUavDescriptor();
            descriptor.dx12CpuDescriptorHandle = allocation.cpuHandle;
            descriptor.dx12GpuDescriptorHandle = allocation.gpuHandle;
            descriptor.dx12DescriptorHeap = allocation.descriptorHeap;
            buffer.GetDevice().GetDX12Device()->CreateConstantBufferView(&desc, descriptor.dx12CpuDescriptorHandle);
        } else if (IsUnorderedAccessBuffer(buffer.GetUsages())) {
            D3D12_UNORDERED_ACCESS_VIEW_DESC desc {};
            desc.Format = DXGI_FORMAT_UNKNOWN;
            desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
            desc.Buffer.FirstElement = createInfo.offset;
            desc.Buffer.NumElements = createInfo.size;

            auto allocation = buffer.GetDevice().AllocateCbvSrvUavDescriptor();
            descriptor.dx12CpuDescriptorHandle = allocation.cpuHandle;
            descriptor.dx12GpuDescriptorHandle = allocation.gpuHandle;
            descriptor.dx12DescriptorHeap = allocation.descriptorHeap;
            buffer.GetDevice().GetDX12Device()->CreateUnorderedAccessView(buffer.GetDX12Resource().Get(), nullptr, &desc, descriptor.dx12CpuDescriptorHandle);
        } else if (IsVertexBuffer(buffer.GetUsages())) {
            vertex.dx12VertexBufferView.BufferLocation = buffer.GetDX12Resource()->GetGPUVirtualAddress() + createInfo.offset;
            vertex.dx12VertexBufferView.SizeInBytes = createInfo.size;
            vertex.dx12VertexBufferView.StrideInBytes = createInfo.vertex.stride;
        } else if (IsIndexBuffer(buffer.GetUsages())) {
            index.dx12IndexBufferView.BufferLocation = buffer.GetDX12Resource()->GetGPUVirtualAddress() + createInfo.offset;
            index.dx12IndexBufferView.SizeInBytes = createInfo.size;
            index.dx12IndexBufferView.Format = DX12EnumCast<IndexFormat, DXGI_FORMAT>(createInfo.index.format);
        }
    }
}
