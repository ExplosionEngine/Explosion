//
// Created by johnk on 10/1/2022.
//

#ifndef EXPLOSION_RHI_DX12_COMMON_H
#define EXPLOSION_RHI_DX12_COMMON_H

#include <stdexcept>
#include <utility>
#include <unordered_map>
#include <format>

#include <dxgi1_4.h>
#include <d3d12.h>

#include <RHI/Enum.h>

namespace RHI::DirectX12 {
    class DX12Exception : public std::exception {
    public:
        explicit DX12Exception(std::string msg) : message(std::move(msg)) {}

        [[nodiscard]] const char* what() const override
        {
            return message.c_str();
        }

    private:
        std::string message;
    };
}

// duplicated code because static variable and namespace
namespace RHI::DirectX12 {
    template <typename A, typename B>
    static const std::unordered_map<A, B> DX12_ENUM_MAP;

    template <typename A, typename B>
    B DX12EnumCast(const A& value)
    {
        auto iter = DX12_ENUM_MAP<A, B>.find(value);
        if (iter == DX12_ENUM_MAP<A, B>.end()) {
            throw DX12Exception(std::format("failed to find suitable enum cast result for {}", typeid(A).name()));
        }
        return static_cast<B>(iter->second);
    }

#define DX12_ENUM_MAP_BEGIN(A, B) template <> static const std::unordered_map<A, B> DX12_ENUM_MAP<A, B> = {
#define DX12_ENUM_MAP_ITEM(A, B) { A, B },
#define DX12_ENUM_MAP_END() };
}

// hard code convert
namespace RHI::DirectX12 {
    GpuType GetGpuTypeByAdapterFlag(UINT flag);
}

// enum cast impl
namespace RHI::DirectX12 {
    DX12_ENUM_MAP_BEGIN(QueueType, D3D12_COMMAND_LIST_TYPE)
        DX12_ENUM_MAP_ITEM(QueueType::GRAPHICS, D3D12_COMMAND_LIST_TYPE_DIRECT)
        DX12_ENUM_MAP_ITEM(QueueType::COMPUTE, D3D12_COMMAND_LIST_TYPE_COMPUTE)
        DX12_ENUM_MAP_ITEM(QueueType::TRANSFER, D3D12_COMMAND_LIST_TYPE_COPY)
    DX12_ENUM_MAP_END()

    DX12_ENUM_MAP_BEGIN(PixelFormat, DXGI_FORMAT)
        // 8-Bits
        DX12_ENUM_MAP_ITEM(PixelFormat::R8_UNORM, DXGI_FORMAT_R8_UNORM)
        DX12_ENUM_MAP_ITEM(PixelFormat::R8_SNORM, DXGI_FORMAT_R8_SNORM)
        DX12_ENUM_MAP_ITEM(PixelFormat::R8_UINT, DXGI_FORMAT_R8_UINT)
        DX12_ENUM_MAP_ITEM(PixelFormat::R8_SINT, DXGI_FORMAT_R8_SINT)
        // 16-Bits
        DX12_ENUM_MAP_ITEM(PixelFormat::R16_UINT, DXGI_FORMAT_R16_UINT)
        DX12_ENUM_MAP_ITEM(PixelFormat::R16_SINT, DXGI_FORMAT_R16_SINT)
        DX12_ENUM_MAP_ITEM(PixelFormat::R16_FLOAT, DXGI_FORMAT_R16_FLOAT)
        DX12_ENUM_MAP_ITEM(PixelFormat::RG8_UNORM, DXGI_FORMAT_R8G8_UNORM)
        DX12_ENUM_MAP_ITEM(PixelFormat::RG8_SNORM, DXGI_FORMAT_R8G8_SNORM)
        DX12_ENUM_MAP_ITEM(PixelFormat::RG8_UINT, DXGI_FORMAT_R8G8_UINT)
        DX12_ENUM_MAP_ITEM(PixelFormat::RG8_SINT, DXGI_FORMAT_R8G8_SINT)
        // 32-Bits
        DX12_ENUM_MAP_ITEM(PixelFormat::R32_UINT, DXGI_FORMAT_R32_UINT)
        DX12_ENUM_MAP_ITEM(PixelFormat::R32_SINT, DXGI_FORMAT_R32_SINT)
        DX12_ENUM_MAP_ITEM(PixelFormat::R32_FLOAT, DXGI_FORMAT_R32_FLOAT)
        DX12_ENUM_MAP_ITEM(PixelFormat::RG16_UINT, DXGI_FORMAT_R16G16_UINT)
        DX12_ENUM_MAP_ITEM(PixelFormat::RG16_SINT, DXGI_FORMAT_R16G16_SINT)
        DX12_ENUM_MAP_ITEM(PixelFormat::RG16_FLOAT, DXGI_FORMAT_R16G16_FLOAT)
        DX12_ENUM_MAP_ITEM(PixelFormat::RGBA8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM)
        DX12_ENUM_MAP_ITEM(PixelFormat::RGBA8_UNORM_SRGB, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB)
        DX12_ENUM_MAP_ITEM(PixelFormat::RGBA8_SNORM, DXGI_FORMAT_R8G8B8A8_SNORM)
        DX12_ENUM_MAP_ITEM(PixelFormat::RGBA8_UINT, DXGI_FORMAT_R8G8B8A8_UINT)
        DX12_ENUM_MAP_ITEM(PixelFormat::RGBA8_SINT, DXGI_FORMAT_R8G8B8A8_SINT)
        DX12_ENUM_MAP_ITEM(PixelFormat::BGRA8_UNORM, DXGI_FORMAT_B8G8R8A8_UNORM)
        DX12_ENUM_MAP_ITEM(PixelFormat::BGRA8_UNORM_SRGB, DXGI_FORMAT_B8G8R8A8_UNORM_SRGB)
        DX12_ENUM_MAP_ITEM(PixelFormat::RGB9_E5_FLOAT, DXGI_FORMAT_R9G9B9E5_SHAREDEXP)
        DX12_ENUM_MAP_ITEM(PixelFormat::RGB10A2_UNORM, DXGI_FORMAT_R10G10B10A2_UNORM)
        DX12_ENUM_MAP_ITEM(PixelFormat::RG11B10_FLOAT, DXGI_FORMAT_R11G11B10_FLOAT)
        // 64-Bits
        DX12_ENUM_MAP_ITEM(PixelFormat::RG32_UINT, DXGI_FORMAT_R32G32_UINT)
        DX12_ENUM_MAP_ITEM(PixelFormat::RG32_SINT, DXGI_FORMAT_R32G32_SINT)
        DX12_ENUM_MAP_ITEM(PixelFormat::RG32_FLOAT, DXGI_FORMAT_R32G32_FLOAT)
        DX12_ENUM_MAP_ITEM(PixelFormat::RGBA16_UINT, DXGI_FORMAT_R16G16B16A16_UINT)
        DX12_ENUM_MAP_ITEM(PixelFormat::RGBA16_SINT, DXGI_FORMAT_R16G16B16A16_SINT)
        DX12_ENUM_MAP_ITEM(PixelFormat::RGBA16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT)
        // 128-Bits
        DX12_ENUM_MAP_ITEM(PixelFormat::RGBA32_UINT, DXGI_FORMAT_R32G32B32A32_UINT)
        DX12_ENUM_MAP_ITEM(PixelFormat::RGBA32_SINT, DXGI_FORMAT_R32G32B32A32_SINT)
        DX12_ENUM_MAP_ITEM(PixelFormat::RGBA32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT)
        // Depth-Stencil
        DX12_ENUM_MAP_ITEM(PixelFormat::D16_UNORM, DXGI_FORMAT_D16_UNORM)
        DX12_ENUM_MAP_ITEM(PixelFormat::D24_UNORM_S8_UINT, DXGI_FORMAT_D24_UNORM_S8_UINT)
        DX12_ENUM_MAP_ITEM(PixelFormat::D32_FLOAT, DXGI_FORMAT_D32_FLOAT)
    DX12_ENUM_MAP_END()

    DX12_ENUM_MAP_BEGIN(TextureViewDimension, D3D12_SRV_DIMENSION)
        DX12_ENUM_MAP_ITEM(TextureViewDimension::TV_1D, D3D12_SRV_DIMENSION_TEXTURE1D)
        DX12_ENUM_MAP_ITEM(TextureViewDimension::TV_2D, D3D12_SRV_DIMENSION_TEXTURE2D)
        DX12_ENUM_MAP_ITEM(TextureViewDimension::TV_2D_ARRAY, D3D12_SRV_DIMENSION_TEXTURE2DARRAY)
        DX12_ENUM_MAP_ITEM(TextureViewDimension::TV_CUBE, D3D12_SRV_DIMENSION_TEXTURECUBE)
        DX12_ENUM_MAP_ITEM(TextureViewDimension::TV_CUBE_ARRAY, D3D12_SRV_DIMENSION_TEXTURECUBEARRAY)
        DX12_ENUM_MAP_ITEM(TextureViewDimension::TV_3D, D3D12_SRV_DIMENSION_TEXTURE3D)
    DX12_ENUM_MAP_END()

    DX12_ENUM_MAP_BEGIN(TextureViewDimension, D3D12_UAV_DIMENSION)
        DX12_ENUM_MAP_ITEM(TextureViewDimension::TV_1D, D3D12_UAV_DIMENSION_TEXTURE1D)
        DX12_ENUM_MAP_ITEM(TextureViewDimension::TV_2D, D3D12_UAV_DIMENSION_TEXTURE2D)
        DX12_ENUM_MAP_ITEM(TextureViewDimension::TV_2D_ARRAY, D3D12_UAV_DIMENSION_TEXTURE2DARRAY)
        DX12_ENUM_MAP_ITEM(TextureViewDimension::TV_3D, D3D12_UAV_DIMENSION_TEXTURE3D)
        // not support cube and cube array when texture is used as a UAV
    DX12_ENUM_MAP_END()

    DX12_ENUM_MAP_BEGIN(TextureViewDimension, D3D12_RTV_DIMENSION)
        DX12_ENUM_MAP_ITEM(TextureViewDimension::TV_1D, D3D12_RTV_DIMENSION_TEXTURE1D)
        DX12_ENUM_MAP_ITEM(TextureViewDimension::TV_2D, D3D12_RTV_DIMENSION_TEXTURE2D)
        DX12_ENUM_MAP_ITEM(TextureViewDimension::TV_2D_ARRAY, D3D12_RTV_DIMENSION_TEXTURE2DARRAY)
        DX12_ENUM_MAP_ITEM(TextureViewDimension::TV_3D, D3D12_RTV_DIMENSION_TEXTURE3D)
    DX12_ENUM_MAP_END()
}

#endif //EXPLOSION_RHI_DX12_COMMON_H
