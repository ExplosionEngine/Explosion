//
// Created by johnk on 2022/2/25.
//

#pragma once

#include <wrl/client.h>
#include <directx/d3dx12.h>
using Microsoft::WRL::ComPtr;

#include <RHI/TextureView.h>

namespace RHI::DirectX12 {
    class DX12Device;
    class DX12Texture;

    class DX12TextureView final : public TextureView {
    public:
        NonCopyable(DX12TextureView)
        explicit DX12TextureView(DX12Device& inDevice, DX12Texture& inTexture, const TextureViewCreateInfo& inCreateInfo);
        ~DX12TextureView() override;

        CD3DX12_CPU_DESCRIPTOR_HANDLE GetNativeCpuDescriptorHandle() const;

    private:
        void CreateNativeDescriptor(DX12Device& inDevice, const TextureViewCreateInfo& inCreateInfo);

        DX12Texture& texture;
        Common::UniquePtr<DescriptorAllocation> descriptorAllocation;
    };
}
