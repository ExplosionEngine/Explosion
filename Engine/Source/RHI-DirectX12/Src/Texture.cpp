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
        : Texture(inCreateInfo), device(inDevice), usages(inCreateInfo.usages), format(inCreateInfo.format)
    {
        CreateNativeTexture(inCreateInfo);
    }

    DX12Texture::DX12Texture(DX12Device& inDevice, PixelFormat inFormat, ComPtr<ID3D12Resource>&& nativeResource)
        : Texture(), device(inDevice), usages(static_cast<TextureUsageFlags>(TextureUsageBits::renderAttachment)), nativeResource(nativeResource), format(inFormat) {}

    DX12Texture::~DX12Texture() = default;

    Common::UniqueRef<TextureView> DX12Texture::CreateTextureView(const TextureViewCreateInfo& inCreateInfo)
    {
        return Common::UniqueRef<TextureView>(new DX12TextureView(static_cast<DX12Device&>(device), *this, inCreateInfo));
    }

    TextureUsageFlags DX12Texture::GetUsages() const
    {
        return usages;
    }

    PixelFormat DX12Texture::GetFormat() const
    {
        return format;
    }

    ID3D12Resource* DX12Texture::GetNative()
    {
        return nativeResource.Get();
    }

    void DX12Texture::CreateNativeTexture(const TextureCreateInfo& inCreateInfo)
    {
        CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
        D3D12_RESOURCE_DESC textureDesc = {};
        textureDesc.MipLevels = inCreateInfo.mipLevels;
        textureDesc.Format = EnumCast<PixelFormat, DXGI_FORMAT>(inCreateInfo.format);
        textureDesc.Width = inCreateInfo.extent.x;
        textureDesc.Height = inCreateInfo.extent.y;
        textureDesc.Flags = FlagsCast<TextureUsageFlags, D3D12_RESOURCE_FLAGS>(inCreateInfo.usages);
        textureDesc.DepthOrArraySize = inCreateInfo.extent.z;
        textureDesc.SampleDesc.Count = inCreateInfo.samples;
        // TODO https://docs.microsoft.com/en-us/windows/win32/api/dxgicommon/ns-dxgicommon-dxgi_sample_desc
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
            nativeResource->SetName(Common::StringUtils::ToWideString(inCreateInfo.debugName).c_str());
        }
#endif
    }
}
