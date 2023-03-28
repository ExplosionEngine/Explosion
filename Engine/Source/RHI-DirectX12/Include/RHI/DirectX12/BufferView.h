//
// Created by johnk on 20/3/2022.
//

#pragma once

#include <wrl/client.h>
#include <directx/d3dx12.h>
using Microsoft::WRL::ComPtr;

#include <RHI/BufferView.h>

namespace RHI::DirectX12 {
    class DX12Buffer;
    class DX12Device;

    class DX12BufferView : public BufferView {
    public:
        NON_COPYABLE(DX12BufferView)
        DX12BufferView(DX12Buffer& buffer, const BufferViewCreateInfo& createInfo);
        ~DX12BufferView() override;

        void Destroy() override;

        ID3D12DescriptorHeap* GetDX12DescriptorHeap() const;
        CD3DX12_CPU_DESCRIPTOR_HANDLE GetDX12CpuDescriptorHandle() const;
        CD3DX12_GPU_DESCRIPTOR_HANDLE GetDX12GpuDescriptorHandle() const;
        [[nodiscard]] const D3D12_VERTEX_BUFFER_VIEW& GetDX12VertexBufferView() const;
        [[nodiscard]] const D3D12_INDEX_BUFFER_VIEW& GetDX12IndexBufferView() const;

    private:
        struct DescriptorBufferInfo {
            ID3D12DescriptorHeap* dx12DescriptorHeap;
            CD3DX12_CPU_DESCRIPTOR_HANDLE dx12CpuDescriptorHandle;
            CD3DX12_GPU_DESCRIPTOR_HANDLE dx12GpuDescriptorHandle;
        };

        struct VertexBufferInfo {
            D3D12_VERTEX_BUFFER_VIEW dx12VertexBufferView;
        };

        struct IndexBufferInfo {
            D3D12_INDEX_BUFFER_VIEW dx12IndexBufferView;
        };

        void CreateDX12Descriptor(const BufferViewCreateInfo& createInfo);

        DX12Buffer& buffer;
        union {
            DescriptorBufferInfo descriptor;
            VertexBufferInfo vertex;
            IndexBufferInfo index;
        };
    };
}
