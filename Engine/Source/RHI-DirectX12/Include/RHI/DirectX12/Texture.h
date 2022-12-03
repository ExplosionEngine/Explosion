//
// Created by johnk on 2022/2/21.
//

#pragma once

#include <RHI/Texture.h>
#include <d3d12.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace RHI::DirectX12 {
    class DX12Device;

    class DX12Texture : public Texture {
    public:
        NON_COPYABLE(DX12Texture)
        explicit DX12Texture(DX12Device& device, const TextureCreateInfo* createInfo);
        explicit DX12Texture(DX12Device& device, ComPtr<ID3D12Resource>&& dx12Res);
        ~DX12Texture() override;

        TextureView* CreateTextureView(const TextureViewCreateInfo* createInfo) override;
        void Destroy() override;

        TextureUsageFlags GetUsages() const;
        PixelFormat GetFormat() const;
        ComPtr<ID3D12Resource>& GetDX12Resource();

    private:
        void CreateDX12Texture(const TextureCreateInfo* createInfo);

        DX12Device& device;
        TextureUsageFlags usages;
        PixelFormat format;
        ComPtr<ID3D12Resource> dx12Resource;
    };
}
