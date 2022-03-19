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
            { TextureDimension::T_1D, D3D12_RESOURCE_DIMENSION_TEXTURE1D },
            { TextureDimension::T_2D, D3D12_RESOURCE_DIMENSION_TEXTURE2D },
            { TextureDimension::T_3D, D3D12_RESOURCE_DIMENSION_TEXTURE3D }
        };
        auto iter = MAP.find(dimension);
        if (iter == MAP.end()) {
            throw DirectX12::DX12Exception("bad texture dimension");
        }
        return iter->second;
    }

    static D3D12_RESOURCE_STATES GetDX12ResourceStates(TextureUsageFlags textureUsages)
    {
        static std::unordered_map<TextureUsageBits, D3D12_RESOURCE_STATES> rules = {
            { TextureUsageBits::COPY_SRC, D3D12_RESOURCE_STATE_COPY_SOURCE },
            { TextureUsageBits::COPY_DST, D3D12_RESOURCE_STATE_COPY_DEST },
            { TextureUsageBits::TEXTURE_BINDING, D3D12_RESOURCE_STATE_COMMON },
            { TextureUsageBits::STORAGE_BINDING, D3D12_RESOURCE_STATE_UNORDERED_ACCESS },
            { TextureUsageBits::RENDER_ATTACHMENT, D3D12_RESOURCE_STATE_RENDER_TARGET },
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
    DX12Texture::DX12Texture(DX12Device& device, const TextureCreateInfo* createInfo)
        : Texture(createInfo), device(device), usages(createInfo->usages)
    {
        CreateDX12Texture(device, createInfo);
    }

    DX12Texture::~DX12Texture() = default;

    TextureView* DX12Texture::CreateTextureView(const TextureViewCreateInfo* createInfo)
    {
        return new DX12TextureView(device, *this, createInfo);
    }

    void DX12Texture::Destroy()
    {
        delete this;
    }

    TextureUsageFlags DX12Texture::GetUsages()
    {
        return usages;
    }

    ComPtr<ID3D12Resource>& DX12Texture::GetDX12Resource()
    {
        return dx12Resource;
    }

    void DX12Texture::CreateDX12Texture(DX12Device& device, const TextureCreateInfo* createInfo)
    {
        CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
        D3D12_RESOURCE_DESC textureDesc = {};
        textureDesc.MipLevels = createInfo->mipLevels;
        textureDesc.Format = DX12EnumCast<PixelFormat, DXGI_FORMAT>(createInfo->format);
        textureDesc.Width = createInfo->extent.x;
        textureDesc.Height = createInfo->extent.y;
        textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
        textureDesc.DepthOrArraySize = createInfo->extent.z;
        textureDesc.SampleDesc.Count = createInfo->samples;
        // TODO https://docs.microsoft.com/en-us/windows/win32/api/dxgicommon/ns-dxgicommon-dxgi_sample_desc
        textureDesc.SampleDesc.Quality = 0;
        textureDesc.Dimension = GetDX12ResourceDimension(createInfo->dimension);

        if (FAILED(device.GetDX12Device()->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &textureDesc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&dx12Resource)))) {
            throw DX12Exception("failed to create texture resource");
        }
    }
}
