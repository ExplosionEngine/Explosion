//
// Created by johnk on 2022/2/21.
//

#pragma once

#include <d3d12.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

#include <RHI/Texture.h>

namespace RHI::DirectX12 {
    class DX12Device;

    class DX12Texture : public Texture {
    public:
        NonCopyable(DX12Texture)
        explicit DX12Texture(DX12Device& inDevice, const TextureCreateInfo& inCreateInfo);
        explicit DX12Texture(DX12Device& inDevice, PixelFormat inFormat, ComPtr<ID3D12Resource>&& nativeResource);
        ~DX12Texture() override;

        Common::UniqueRef<TextureView> CreateTextureView(const TextureViewCreateInfo& inCreateInfo) override;

        TextureUsageFlags GetUsages() const;
        PixelFormat GetFormat() const;
        ID3D12Resource* GetNative();

    private:
        void CreateNativeTexture(const TextureCreateInfo& inCreateInfo);

        DX12Device& device;
        TextureUsageFlags usages;
        PixelFormat format;
        ComPtr<ID3D12Resource> nativeResource;
    };
}
