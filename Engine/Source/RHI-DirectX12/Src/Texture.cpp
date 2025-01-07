//
// Created by johnk on 2022/2/21.
//

#include <directx/d3dx12.h>

#include <RHI/DirectX12/Common.h>
#include <RHI/DirectX12/Device.h>
#include <RHI/DirectX12/Texture.h>
#include <RHI/DirectX12/TextureView.h>

namespace RHI::DirectX12 {
    DX12Texture::DX12Texture(DX12Device& inDevice, const TextureCreateInfo& inCreateInfo)
        : Texture(inCreateInfo)
        , device(inDevice)
    {
        CreateNativeTexture(inCreateInfo);
    }

    DX12Texture::DX12Texture(DX12Device& inDevice, const TextureCreateInfo& inCreateInfo, ComPtr<ID3D12Resource>&& nativeResource)
        : Texture(inCreateInfo)
        , device(inDevice)
        , nativeResource(nativeResource)
    {
    }

    DX12Texture::~DX12Texture() = default;

    Common::UniqueRef<TextureView> DX12Texture::CreateTextureView(const TextureViewCreateInfo& inCreateInfo)
    {
        return Common::UniqueRef<TextureView>(new DX12TextureView(static_cast<DX12Device&>(device), *this, inCreateInfo));
    }

    ID3D12Resource* DX12Texture::GetNative() const
    {
        return nativeResource.Get();
    }

    void DX12Texture::CreateNativeTexture(const TextureCreateInfo& inCreateInfo)
    {
        const CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
        D3D12_RESOURCE_DESC textureDesc = {};
        textureDesc.MipLevels = inCreateInfo.mipLevels;
        textureDesc.Format = EnumCast<PixelFormat, DXGI_FORMAT>(inCreateInfo.format);
        textureDesc.Width = inCreateInfo.width;
        textureDesc.Height = inCreateInfo.height;
        textureDesc.Flags = FlagsCast<TextureUsageFlags, D3D12_RESOURCE_FLAGS>(inCreateInfo.usages);
        textureDesc.DepthOrArraySize = inCreateInfo.depthOrArraySize;
        textureDesc.SampleDesc.Count = inCreateInfo.samples;
        textureDesc.SampleDesc.Quality = 0;
        textureDesc.Dimension = EnumCast<TextureDimension, D3D12_RESOURCE_DIMENSION>(inCreateInfo.dimension);

        bool success = SUCCEEDED(device.GetNative()->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &textureDesc,
            EnumCast<TextureState, D3D12_RESOURCE_STATES>(inCreateInfo.initialState),
            nullptr,
            IID_PPV_ARGS(&nativeResource)));
        Assert(success);

#if BUILD_CONFIG_DEBUG
        if (!inCreateInfo.debugName.empty()) {
            Assert(SUCCEEDED(nativeResource->SetName(Common::StringUtils::ToWideString(inCreateInfo.debugName).c_str())));
        }
#endif
    }
}
