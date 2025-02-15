//
// Created by johnk on 20/3/2022.
//

#pragma once

#include <wrl/client.h>
#include <directx/d3dx12.h>
using Microsoft::WRL::ComPtr;

#include <RHI/BufferView.h>
#include <RHI/DirectX12/Device.h>

namespace RHI::DirectX12 {
    class DX12Buffer;
    class DX12Device;

    class DX12BufferView final : public BufferView {
    public:
        NonCopyable(DX12BufferView)
        DX12BufferView(DX12Buffer& inBuffer, const BufferViewCreateInfo& inCreateInfo);
        ~DX12BufferView() override;

        CD3DX12_CPU_DESCRIPTOR_HANDLE GetNativeCpuDescriptorHandle() const;
        [[nodiscard]] const D3D12_VERTEX_BUFFER_VIEW& GetNativeVertexBufferView() const;
        [[nodiscard]] const D3D12_INDEX_BUFFER_VIEW& GetNativeIndexBufferView() const;

    private:
        void CreateNativeView(const BufferViewCreateInfo& inCreateInfo);

        DX12Buffer& buffer;
        std::variant<Common::UniquePtr<DescriptorAllocation>, D3D12_VERTEX_BUFFER_VIEW, D3D12_INDEX_BUFFER_VIEW> nativeView;
    };
}
