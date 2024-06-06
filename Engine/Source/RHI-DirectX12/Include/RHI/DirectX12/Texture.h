//
// Created by johnk on 2022/2/21.
//

#pragma once

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#include <RHI/Texture.h>

namespace RHI::DirectX12 {
    class DX12Device;

    class DX12Texture : public Texture {
    public:
        NonCopyable(DX12Texture)
        DX12Texture(DX12Device& inDevice, const TextureCreateInfo& inCreateInfo);
        DX12Texture(DX12Device& inDevice, const TextureCreateInfo& inCreateInfo, ComPtr<ID3D12Resource>&& nativeResource);
        ~DX12Texture() override;

        Common::UniqueRef<TextureView> CreateTextureView(const TextureViewCreateInfo& inCreateInfo) override;

        ID3D12Resource* GetNative() const;

    private:
        void CreateNativeTexture(const TextureCreateInfo& inCreateInfo);

        DX12Device& device;
        ComPtr<ID3D12Resource> nativeResource;
    };
}
