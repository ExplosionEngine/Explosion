//
// Created by johnk on 2022/2/21.
//

#ifndef EXPLOSION_RHI_DX12_TEXTURE_H
#define EXPLOSION_RHI_DX12_TEXTURE_H

#include <RHI/Texture.h>
#include <d3d12.h>
#include <wrl/client.h>

using namespace Microsoft::WRL;

namespace RHI::DirectX12 {
    class DX12Device;

    class DX12Texture : public Texture {
    public:
        NON_COPYABLE(DX12Texture)
        explicit DX12Texture(DX12Device& device, const TextureCreateInfo* createInfo);
        ~DX12Texture() override;

        TextureView* CreateTextureView(const TextureViewCreateInfo* createInfo) override;
        void Destroy() override;

        TextureUsageFlags GetUsages();
        ComPtr<ID3D12Resource>& GetDX12Resource();

    private:
        void CreateDX12Texture(DX12Device& device, const TextureCreateInfo* createInfo);

        TextureUsageFlags usages;
        ComPtr<ID3D12Resource> dx12Resource;
    };
}

#endif //EXPLOSION_RHI_DX12_TEXTURE_H
