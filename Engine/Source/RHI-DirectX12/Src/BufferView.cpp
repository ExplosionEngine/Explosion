//
// Created by johnk on 20/3/2022.
//

#include <utility>

#include <RHI/DirectX12/Device.h>
#include <RHI/DirectX12/Buffer.h>
#include <RHI/DirectX12/BufferView.h>
#include <RHI/DirectX12/Common.h>

namespace RHI::DirectX12 {
    static uint32_t GetStrideOfStorageBuffer(StorageFormat format)
    {
        if (format == StorageFormat::float32 ||
            format == StorageFormat::sint32 ||
            format == StorageFormat::uint32) {
            return 4;
        }

        return -1;
    }
}

namespace RHI::DirectX12 {
    DX12BufferView::DX12BufferView(DX12Buffer& inBuffer, const BufferViewCreateInfo& inCreateInfo)
        : BufferView(inCreateInfo), buffer(inBuffer)
    {
        CreateNativeView(inCreateInfo);
    }

    DX12BufferView::~DX12BufferView() = default;

    CD3DX12_CPU_DESCRIPTOR_HANDLE DX12BufferView::GetNativeCpuDescriptorHandle() const
    {
        return std::get<Common::UniqueRef<DescriptorAllocation>>(nativeView)->GetCpuHandle();
    }

    const D3D12_VERTEX_BUFFER_VIEW& DX12BufferView::GetNativeVertexBufferView() const
    {
        return std::get<D3D12_VERTEX_BUFFER_VIEW>(nativeView);
    }

    const D3D12_INDEX_BUFFER_VIEW& DX12BufferView::GetNativeIndexBufferView() const
    {
        return std::get<D3D12_INDEX_BUFFER_VIEW>(nativeView);
    }

    void DX12BufferView::CreateNativeView(const BufferViewCreateInfo& inCreateInfo)
    {
        const auto bufferUsages = buffer.GetUsages();

        if (inCreateInfo.type == BufferViewType::uniformBinding) {
            Assert((bufferUsages & BufferUsageBits::uniform) != 0);

            D3D12_CONSTANT_BUFFER_VIEW_DESC desc {};
            desc.BufferLocation = buffer.GetNative()->GetGPUVirtualAddress() + inCreateInfo.offset;
            desc.SizeInBytes = Common::AlignUp<D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT>(inCreateInfo.size);

            nativeView = buffer.GetDevice().AllocateCbvSrvUavDescriptor();
            buffer.GetDevice().GetNative()->CreateConstantBufferView(&desc, std::get<Common::UniqueRef<DescriptorAllocation>>(nativeView)->GetCpuHandle());
        } else if (inCreateInfo.type == BufferViewType::storageBinding) {
            Assert((bufferUsages & BufferUsageBits::storage) != 0);
            auto storageViewInfo = std::get<StorageBufferViewInfo>(inCreateInfo.extend);

            // TODO: check the uav typed load
            D3D12_UNORDERED_ACCESS_VIEW_DESC desc {};
            desc.Format = EnumCast<StorageFormat, DXGI_FORMAT>(storageViewInfo.format);
            desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
            desc.Buffer.FirstElement = inCreateInfo.offset;
            desc.Buffer.NumElements = inCreateInfo.size / GetStrideOfStorageBuffer(storageViewInfo.format);

            nativeView = buffer.GetDevice().AllocateCbvSrvUavDescriptor();
            buffer.GetDevice().GetNative()->CreateUnorderedAccessView(buffer.GetNative(), nullptr, &desc, std::get<Common::UniqueRef<DescriptorAllocation>>(nativeView)->GetCpuHandle());
        } else if (inCreateInfo.type == BufferViewType::vertex) {
            Assert((bufferUsages & BufferUsageBits::vertex) != 0);

            nativeView = D3D12_VERTEX_BUFFER_VIEW();
            D3D12_VERTEX_BUFFER_VIEW& vertexBufferView = std::get<D3D12_VERTEX_BUFFER_VIEW>(nativeView);
            vertexBufferView.BufferLocation = buffer.GetNative()->GetGPUVirtualAddress() + inCreateInfo.offset;
            vertexBufferView.SizeInBytes = inCreateInfo.size;
            vertexBufferView.StrideInBytes = std::get<VertexBufferViewInfo>(inCreateInfo.extend).stride;
        } else if (inCreateInfo.type == BufferViewType::index) {
            Assert((bufferUsages & BufferUsageBits::index) != 0);

            nativeView = D3D12_INDEX_BUFFER_VIEW();
            D3D12_INDEX_BUFFER_VIEW& indexBufferView = std::get<D3D12_INDEX_BUFFER_VIEW>(nativeView);
            indexBufferView.BufferLocation = buffer.GetNative()->GetGPUVirtualAddress() + inCreateInfo.offset;
            indexBufferView.SizeInBytes = inCreateInfo.size;
            indexBufferView.Format = EnumCast<IndexFormat, DXGI_FORMAT>(std::get<IndexBufferViewInfo>(inCreateInfo.extend).format);
        } else {
            Unimplement();
        }
    }
}
