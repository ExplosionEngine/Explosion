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
}

namespace RHI::DirectX12 {
    DX12Texture::DX12Texture(DX12Device& device, const TextureCreateInfo& createInfo)
        : Texture(createInfo), device(device), usages(createInfo.usages), format(createInfo.format)
    {
        CreateDX12Texture(createInfo);
    }

    DX12Texture::DX12Texture(DX12Device& device, PixelFormat inFormat, ComPtr<ID3D12Resource>&& dx12Res)
        : Texture(), device(device), usages(static_cast<TextureUsageFlags>(TextureUsageBits::renderAttachment)), dx12Resource(dx12Res), format(inFormat) {}

    DX12Texture::~DX12Texture() = default;

    TextureView* DX12Texture::CreateTextureView(const TextureViewCreateInfo& createInfo)
    {
        return new DX12TextureView(dynamic_cast<DX12Device&>(device), *this, createInfo);
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

    ComPtr<ID3D12Resource>& DX12Texture::GetDX12Resource()
    {
        return dx12Resource;
    }

    void DX12Texture::CreateDX12Texture(const TextureCreateInfo& createInfo)
    {
        CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
        D3D12_RESOURCE_DESC textureDesc = {};
        textureDesc.MipLevels = createInfo.mipLevels;
        textureDesc.Format = DX12EnumCast<PixelFormat, DXGI_FORMAT>(createInfo.format);
        textureDesc.Width = createInfo.extent.x;
        textureDesc.Height = createInfo.extent.y;
        textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
        textureDesc.DepthOrArraySize = createInfo.extent.z;
        textureDesc.SampleDesc.Count = createInfo.samples;
        // TODO https://docs.microsoft.com/en-us/windows/win32/api/dxgicommon/ns-dxgicommon-dxgi_sample_desc
        textureDesc.SampleDesc.Quality = 0;
        textureDesc.Dimension = GetDX12ResourceDimension(createInfo.dimension);

        bool success = SUCCEEDED(device.GetDX12Device()->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &textureDesc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&dx12Resource)));
        Assert(success);
    }
}
