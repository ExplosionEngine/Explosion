//
// Created by johnk on 2022/2/25.
//

#pragma once

#include <memory>

#include <wrl/client.h>
#include <directx/d3dx12.h>
using Microsoft::WRL::ComPtr;

#include <RHI/TextureView.h>

namespace RHI::DirectX12 {
    class DX12Device;
    class DX12Texture;

    class DX12TextureView : public TextureView {
    public:
        NonCopyable(DX12TextureView)
        explicit DX12TextureView(DX12Device& inDevice, DX12Texture& inTexture, const TextureViewCreateInfo& inCreateInfo);
        ~DX12TextureView() override;

        void Destroy() override;

        CD3DX12_CPU_DESCRIPTOR_HANDLE GetNativeCpuDescriptorHandle();

    private:
        void CreateNativeDescriptor(DX12Device& inDevice, const TextureViewCreateInfo& inCreateInfo);

        DX12Texture& texture;
        CD3DX12_CPU_DESCRIPTOR_HANDLE nativeCpuDescriptorHandle;
    };
}
