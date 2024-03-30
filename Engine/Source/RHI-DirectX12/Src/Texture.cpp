//
// Created by johnk on 2022/2/21.
//

#include <directx/d3dx12.h>

#include <RHI/DirectX12/Common.h>
#include <RHI/DirectX12/Device.h>
#include <RHI/DirectX12/Texture.h>
#include <RHI/DirectX12/TextureView.h>

namespace RHI::DirectX12 {
    static D3D12_RESOURCE_DIMENSION GetDX12ResourceDimension(const TextureDimension& dimension)
    {
        static std::unordered_map<TextureDimension, D3D12_RESOURCE_DIMENSION> MAP = {
            { TextureDimension::t1D, D3D12_RESOURCE_DIMENSION_TEXTURE1D },
            { TextureDimension::t2D, D3D12_RESOURCE_DIMENSION_TEXTURE2D },
            { TextureDimension::t3D, D3D12_RESOURCE_DIMENSION_TEXTURE3D }
        };
        auto iter = MAP.find(dimension);
        Assert(iter != MAP.end());
        return iter->second;
    }

    static D3D12_RESOURCE_STATES GetDX12ResourceStates(TextureUsageFlags textureUsages)
    {
        static std::unordered_map<TextureUsageBits, D3D12_RESOURCE_STATES> rules = {
            { TextureUsageBits::copySrc, D3D12_RESOURCE_STATE_COPY_SOURCE },
            { TextureUsageBits::copyDst, D3D12_RESOURCE_STATE_COPY_DEST },
            { TextureUsageBits::textureBinding, D3D12_RESOURCE_STATE_COMMON },
            { TextureUsageBits::storageBinding, D3D12_RESOURCE_STATE_UNORDERED_ACCESS },
            { TextureUsageBits::renderAttachment, D3D12_RESOURCE_STATE_RENDER_TARGET },
        };

        D3D12_RESOURCE_STATES result = D3D12_RESOURCE_STATE_COMMON;
        for (const auto& rule : rules) {
            if (textureUsages & rule.first) {
                result |= rule.second;
            }
        }
        return result;
    }
    static D3D12_RESOURCE_FLAGS GetDX12ResourceFlags(TextureUsageFlags textureUsages) {
        static std::unordered_map<TextureUsageBits, D3D12_RESOURCE_FLAGS> rules = {
            { TextureUsageBits::copySrc, D3D12_RESOURCE_FLAG_NONE },
            { TextureUsageBits::copyDst, D3D12_RESOURCE_FLAG_NONE },
            { TextureUsageBits::textureBinding, D3D12_RESOURCE_FLAG_NONE },
            { TextureUsageBits::storageBinding, D3D12_RESOURCE_FLAG_NONE },
            { TextureUsageBits::renderAttachment, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET },
            { TextureUsageBits::depthStencilAttachment, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL },
        };

        D3D12_RESOURCE_FLAGS result = D3D12_RESOURCE_FLAG_NONE;
        for (const auto& rule : rules) {
            if (textureUsages & rule.first) {
                result |= rule.second;
            }
        }
        return result;
    }
}

namespace RHI::DirectX12 {
    DX12Texture::DX12Texture(DX12Device& inDevice, const TextureCreateInfo& inCreateInfo)
        : Texture(inCreateInfo), device(inDevice), usages(inCreateInfo.usages), format(inCreateInfo.format)
    {
        CreateNativeTexture(inCreateInfo);
    }

    DX12Texture::DX12Texture(DX12Device& inDevice, PixelFormat inFormat, ComPtr<ID3D12Resource>&& nativeResource)
        : Texture(), device(inDevice), usages(static_cast<TextureUsageFlags>(TextureUsageBits::renderAttachment)), nativeResource(nativeResource), format(inFormat) {}

    DX12Texture::~DX12Texture() = default;

    TextureView* DX12Texture::CreateTextureView(const TextureViewCreateInfo& inCreateInfo)
    {
        return new DX12TextureView(static_cast<DX12Device&>(device), *this, inCreateInfo);
    }

    void DX12Texture::Destroy()
    {
        delete this;
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
        textureDesc.Format = DX12EnumCast<PixelFormat, DXGI_FORMAT>(inCreateInfo.format);
        textureDesc.Width = inCreateInfo.extent.x;
        textureDesc.Height = inCreateInfo.extent.y;
        textureDesc.Flags = GetDX12ResourceFlags(inCreateInfo.usages);
        textureDesc.DepthOrArraySize = inCreateInfo.extent.z;
        textureDesc.SampleDesc.Count = inCreateInfo.samples;
        // TODO https://docs.microsoft.com/en-us/windows/win32/api/dxgicommon/ns-dxgicommon-dxgi_sample_desc
        textureDesc.SampleDesc.Quality = 0;
        textureDesc.Dimension = GetDX12ResourceDimension(inCreateInfo.dimension);

        bool success = SUCCEEDED(device.GetNative()->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &textureDesc,
            DX12EnumCast<TextureState, D3D12_RESOURCE_STATES>(inCreateInfo.initialState),
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
