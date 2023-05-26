//
// Created by johnk on 2022/2/25.
//

#include <RHI/Common.h>
#include <RHI/DirectX12/Common.h>
#include <RHI/DirectX12/Device.h>
#include <RHI/DirectX12/Texture.h>
#include <RHI/DirectX12/TextureView.h>

namespace RHI::DirectX12 {
    static bool IsShaderResource(TextureViewType type)
    {
        return (type == TextureViewType::textureBinding);
    }

    static bool IsUnorderedAccess(TextureViewType type)
    {
        return (type == TextureViewType::storageBinding);
    }

    static bool IsRenderTarget(TextureViewType type)
    {
        return (type == TextureViewType::colorAttachment);
    }

    static bool IsDepthStencil(TextureViewType type)
    {
        return (type == TextureViewType::depthStencil);
    }

    static void FillTexture1DSRV(D3D12_TEX1D_SRV& srv, const TextureViewCreateInfo& createInfo)
    {
        if (createInfo.dimension != TextureViewDimension::tv1D) {
            return;
        }
        srv.MostDetailedMip = createInfo.baseMipLevel;
        srv.MipLevels = createInfo.mipLevelNum;
        srv.ResourceMinLODClamp = static_cast<float>(createInfo.baseMipLevel);
    }

    static void FillTexture2DSRV(D3D12_TEX2D_SRV& srv, const TextureViewCreateInfo& createInfo)
    {
        if (createInfo.dimension != TextureViewDimension::tv2D) {
            return;
        }
        srv.MostDetailedMip = createInfo.baseMipLevel;
        srv.MipLevels = createInfo.mipLevelNum;
        srv.PlaneSlice = 0;
        srv.ResourceMinLODClamp = static_cast<float>(createInfo.baseMipLevel);
    }

    static void FillTexture2DArraySRV(D3D12_TEX2D_ARRAY_SRV& srv, const TextureViewCreateInfo& createInfo)
    {
        if (createInfo.dimension != TextureViewDimension::tv2DArray) {
            return;
        }
        srv.MostDetailedMip = createInfo.baseMipLevel;
        srv.MipLevels = createInfo.mipLevelNum;
        srv.FirstArraySlice = createInfo.baseArrayLayer;
        srv.ArraySize = createInfo.arrayLayerNum;
        srv.PlaneSlice = 0;
        srv.ResourceMinLODClamp = static_cast<float>(createInfo.baseMipLevel);
    }

    static void FillTextureCubeSRV(D3D12_TEXCUBE_SRV& srv, const TextureViewCreateInfo& createInfo)
    {
        if (createInfo.dimension != TextureViewDimension::tvCube) {
            return;
        }
        srv.MostDetailedMip = createInfo.baseMipLevel;
        srv.MipLevels = createInfo.mipLevelNum;
        srv.ResourceMinLODClamp = static_cast<float>(createInfo.baseMipLevel);
    }

    static void FillTextureCubeArraySRV(D3D12_TEXCUBE_ARRAY_SRV& srv, const TextureViewCreateInfo& createInfo)
    {
        if (createInfo.dimension != TextureViewDimension::tvCubeArray) {
            return;
        }
        srv.MostDetailedMip = createInfo.baseMipLevel;
        srv.MipLevels = createInfo.mipLevelNum;
        srv.First2DArrayFace = createInfo.baseArrayLayer;
        srv.NumCubes = createInfo.arrayLayerNum;
        srv.ResourceMinLODClamp = static_cast<float>(createInfo.baseMipLevel);
    }

    static void FillTexture3DSRV(D3D12_TEX3D_SRV& srv, const TextureViewCreateInfo& createInfo)
    {
        if (createInfo.dimension != TextureViewDimension::tv3D) {
            return;
        }
        srv.MostDetailedMip = createInfo.baseMipLevel;
        srv.MipLevels = createInfo.mipLevelNum;
        srv.ResourceMinLODClamp = static_cast<float>(createInfo.baseMipLevel);
    }

    static void FillTexture1DUAV(D3D12_TEX1D_UAV& uav, const TextureViewCreateInfo& createInfo)
    {
        if (createInfo.dimension != TextureViewDimension::tv1D) {
            return;
        }
        uav.MipSlice = createInfo.baseMipLevel;
    }

    static void FillTexture2DUAV(D3D12_TEX2D_UAV& uav, const TextureViewCreateInfo& createInfo)
    {
        if (createInfo.dimension != TextureViewDimension::tv2D) {
            return;
        }
        uav.MipSlice = createInfo.baseMipLevel;
        uav.PlaneSlice = 0;
    }

    static void FillTexture2DArrayUAV(D3D12_TEX2D_ARRAY_UAV& uav, const TextureViewCreateInfo& createInfo)
    {
        if (createInfo.dimension != TextureViewDimension::tv2DArray) {
            return;
        }
        uav.MipSlice = createInfo.baseMipLevel;
        uav.FirstArraySlice = createInfo.baseArrayLayer;
        uav.ArraySize = createInfo.arrayLayerNum;
        uav.PlaneSlice = 0;
    }

    static void FillTexture3DUAV(D3D12_TEX3D_UAV& uav, const TextureViewCreateInfo& createInfo)
    {
        if (createInfo.dimension != TextureViewDimension::tv3D) {
            return;
        }
        uav.MipSlice = createInfo.baseMipLevel;
        uav.FirstWSlice = createInfo.baseArrayLayer;
        uav.WSize = createInfo.arrayLayerNum;
    }

    static void FillTexture1DRTV(D3D12_TEX1D_RTV& rtv, const TextureViewCreateInfo& createInfo)
    {
        if (createInfo.dimension != TextureViewDimension::tv1D) {
            return;
        }
        rtv.MipSlice = createInfo.baseMipLevel;
    }

    static void FillTexture2DRTV(D3D12_TEX2D_RTV& rtv, const TextureViewCreateInfo& createInfo)
    {
        if (createInfo.dimension != TextureViewDimension::tv2D) {
            return;
        }
        rtv.MipSlice = createInfo.baseMipLevel;
        rtv.PlaneSlice = 0;
    }

    static void FillTexture2DArrayRTV(D3D12_TEX2D_ARRAY_RTV& rtv, const TextureViewCreateInfo& createInfo)
    {
        if (createInfo.dimension != TextureViewDimension::tv2DArray) {
            return;
        }
        rtv.MipSlice = createInfo.baseMipLevel;
        rtv.FirstArraySlice = createInfo.baseArrayLayer;
        rtv.ArraySize = createInfo.arrayLayerNum;
        rtv.PlaneSlice = 0;
    }

    static void FillTexture3DRTV(D3D12_TEX3D_RTV& rtv, const TextureViewCreateInfo& createInfo)
    {
        if (createInfo.dimension != TextureViewDimension::tv3D) {
            return;
        }
        rtv.MipSlice = createInfo.baseMipLevel;
        rtv.FirstWSlice = createInfo.baseArrayLayer;
        rtv.WSize = createInfo.arrayLayerNum;
    }

    static void FillTexture1DDSV(D3D12_TEX1D_DSV& dsv, const TextureViewCreateInfo& createInfo)
    {
        if (createInfo.dimension != TextureViewDimension::tv1D) {
            return;
        }
        dsv.MipSlice = createInfo.baseMipLevel;
    }

    static void FillTexture2DDSV(D3D12_TEX2D_DSV& dsv, const TextureViewCreateInfo& createInfo)
    {
        if (createInfo.dimension != TextureViewDimension::tv2D) {
            return;
        }
        dsv.MipSlice = createInfo.baseMipLevel;
    }

    static void FillTexture2DArrayDSV(D3D12_TEX2D_ARRAY_DSV& dsv, const TextureViewCreateInfo& createInfo)
    {
        if (createInfo.dimension != TextureViewDimension::tv2DArray) {
            return;
        }
        dsv.MipSlice = createInfo.baseMipLevel;
        dsv.FirstArraySlice = createInfo.baseArrayLayer;
        dsv.ArraySize = createInfo.arrayLayerNum;
    }
}

namespace RHI::DirectX12 {
    DX12TextureView::DX12TextureView(DX12Device& device, DX12Texture& texture, const TextureViewCreateInfo& createInfo)
        : TextureView(createInfo), texture(texture), dx12DescriptorHeap(nullptr), dx12CpuDescriptorHandle(), dx12GpuDescriptorHandle()
    {
        CreateDX12Descriptor(device, createInfo);
    }

    DX12TextureView::~DX12TextureView() = default;

    void DX12TextureView::Destroy()
    {
        delete this;
    }

    CD3DX12_CPU_DESCRIPTOR_HANDLE DX12TextureView::GetDX12CpuDescriptorHandle()
    {
        return dx12CpuDescriptorHandle;
    }

    CD3DX12_GPU_DESCRIPTOR_HANDLE DX12TextureView::GetDX12GpuDescriptorHandle()
    {
        return dx12GpuDescriptorHandle;
    }

    ID3D12DescriptorHeap* DX12TextureView::GetDX12DescriptorHeap()
    {
        return dx12DescriptorHeap;
    }

    void DX12TextureView::CreateDX12Descriptor(DX12Device& device, const TextureViewCreateInfo& createInfo)
    {
        if (IsShaderResource(createInfo.type)) {
            D3D12_SHADER_RESOURCE_VIEW_DESC desc {};
            desc.Format = DX12EnumCast<PixelFormat, DXGI_FORMAT>(texture.GetFormat());
            desc.ViewDimension = DX12EnumCast<TextureViewDimension, D3D12_SRV_DIMENSION>(createInfo.dimension);
            desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            FillTexture1DSRV(desc.Texture1D, createInfo);
            FillTexture2DSRV(desc.Texture2D, createInfo);
            FillTexture2DArraySRV(desc.Texture2DArray, createInfo);
            FillTextureCubeSRV(desc.TextureCube, createInfo);
            FillTextureCubeArraySRV(desc.TextureCubeArray, createInfo);
            FillTexture3DSRV(desc.Texture3D, createInfo);

            auto allocation = device.AllocateCbvSrvUavDescriptor();
            dx12CpuDescriptorHandle = allocation.cpuHandle;
            dx12GpuDescriptorHandle = allocation.gpuHandle;
            dx12DescriptorHeap = allocation.descriptorHeap;
            device.GetDX12Device()->CreateShaderResourceView(texture.GetDX12Resource().Get(), &desc, dx12CpuDescriptorHandle);
        } else if (IsUnorderedAccess(createInfo.type)) {
            D3D12_UNORDERED_ACCESS_VIEW_DESC desc {};
            desc.Format = DX12EnumCast<PixelFormat, DXGI_FORMAT>(texture.GetFormat());
            desc.ViewDimension = DX12EnumCast<TextureViewDimension, D3D12_UAV_DIMENSION>(createInfo.dimension);
            FillTexture1DUAV(desc.Texture1D, createInfo);
            FillTexture2DUAV(desc.Texture2D, createInfo);
            FillTexture2DArrayUAV(desc.Texture2DArray, createInfo);
            FillTexture3DUAV(desc.Texture3D, createInfo);

            auto allocation = device.AllocateCbvSrvUavDescriptor();
            dx12CpuDescriptorHandle = allocation.cpuHandle;
            dx12GpuDescriptorHandle = allocation.gpuHandle;
            dx12DescriptorHeap = allocation.descriptorHeap;
            device.GetDX12Device()->CreateUnorderedAccessView(texture.GetDX12Resource().Get(), nullptr, &desc, dx12CpuDescriptorHandle);
        } else if (IsRenderTarget(createInfo.type)) {
            D3D12_RENDER_TARGET_VIEW_DESC desc {};
            desc.Format = DX12EnumCast<PixelFormat, DXGI_FORMAT>(texture.GetFormat());
            desc.ViewDimension = DX12EnumCast<TextureViewDimension, D3D12_RTV_DIMENSION>(createInfo.dimension);
            FillTexture1DRTV(desc.Texture1D, createInfo);
            FillTexture2DRTV(desc.Texture2D, createInfo);
            FillTexture2DArrayRTV(desc.Texture2DArray, createInfo);
            FillTexture3DRTV(desc.Texture3D, createInfo);

            auto allocation = device.AllocateRtvDescriptor();
            dx12CpuDescriptorHandle = allocation.cpuHandle;
            dx12GpuDescriptorHandle = allocation.gpuHandle;
            dx12DescriptorHeap = allocation.descriptorHeap;
            device.GetDX12Device()->CreateRenderTargetView(texture.GetDX12Resource().Get(), &desc, dx12CpuDescriptorHandle);
        } else if (IsDepthStencil(createInfo.type)) {
            D3D12_DEPTH_STENCIL_VIEW_DESC desc {};
            desc.Format = DX12EnumCast<PixelFormat, DXGI_FORMAT>(texture.GetFormat());
            desc.ViewDimension = DX12EnumCast<TextureViewDimension, D3D12_DSV_DIMENSION>(createInfo.dimension);
            FillTexture1DDSV(desc.Texture1D, createInfo);
            FillTexture2DDSV(desc.Texture2D, createInfo);
            FillTexture2DArrayDSV(desc.Texture2DArray, createInfo);

            auto allocation = device.AllocateDsvDescriptor();
            dx12CpuDescriptorHandle = allocation.cpuHandle;
            dx12GpuDescriptorHandle = allocation.gpuHandle;
            dx12DescriptorHeap = allocation.descriptorHeap;
            device.GetDX12Device()->CreateDepthStencilView(texture.GetDX12Resource().Get(), &desc, dx12CpuDescriptorHandle);
        }
    }
}
