//
// Created by johnk on 10/1/2022.
//

#ifndef EXPLOSION_RHI_ENUM_H
#define EXPLOSION_RHI_ENUM_H

#include <cstdint>
#include <type_traits>

namespace RHI {
    using EnumType = uint32_t;

    enum class RHIType : EnumType {
        DIRECTX_12,
        VULKAN,
        METAL,
        GNN,
        MAX
    };

    enum class GpuType : EnumType {
        HARDWARE,
        SOFTWARE,
        MAX
    };

    enum class QueueType : EnumType {
        GRAPHICS,
        COMPUTE,
        TRANSFER,
        MAX
    };

    enum class MapMode : EnumType {
        READ,
        WRITE,
        MAX
    };

    enum class PixelFormat : EnumType {
        // 8-Bits
        R8_UNORM,
        R8_SNORM,
        R8_UINT,
        R8_SINT,
        // 16-Bits
        R16_UINT,
        R16_SINT,
        R16_FLOAT,
        RG8_UNORM,
        RG8_SNORM,
        RG8_UINT,
        RG8_SINT,
        // 32-Bits
        R32_UINT,
        R32_SINT,
        R32_FLOAT,
        RG16_UINT,
        RG16_SINT,
        RG16_FLOAT,
        RGBA8_UNORM,
        RGBA8_UNORM_SRGB,
        RGBA8_SNORM,
        RGBA8_UINT,
        RGBA8_SINT,
        BGRA8_UNORM,
        BGRA8_UNORM_SRGB,
        RGB9_E5_UFLOAT,
        RGB10A2_UNORM,
        RG11B10_UFLOAT,
        // 64-Bits
        RG32_UINT,
        RG32_SINT,
        RG32_FLOAT,
        RGBA16_UINT,
        RGBA16_SINT,
        RGBA16_FLOAT,
        // 128-Bits
        RGBA32_UINT,
        RGBA32_SINT,
        RGBA32_FLOAT,
        // Depth-Stencil
        STENCIL_8,
        DEPTH_16_UNORM,
        DEPTH_24_PLUS,
        DEPTH_24_PLUS_STENCIL_8,
        DEPTH_32_FLOAT,
        // TODO features / bc / etc / astc
        MAX
    };

    enum class TextureDimension : EnumType {
        T_1D,
        T_2D,
        T_3D,
        MAX
    };

    enum class TextureViewDimension : EnumType {
        TV_1D,
        TV_2D,
        TV_2D_ARRAY,
        TV_CUBE,
        TV_CUBE_ARRAY,
        TV_3D,
        MAX
    };

    enum class TextureAspect : EnumType {
        ALL,
        STENCIL_ONLY,
        DEPTH_ONLY,
        MAX
    };
}

namespace RHI {
    using Flags = uint32_t;

    template <typename T> concept IsFlagsType = std::is_same_v<T, Flags>;
    template <typename T> concept IsBitsType = std::is_enum_v<T> && std::is_same_v<std::underlying_type_t<T>, Flags>;
    template <typename T> concept IsFlagsOrBitsType = IsFlagsType<T> || IsBitsType<T>;

    template <typename TA, typename TB>
    requires IsFlagsOrBitsType<TA> && IsFlagsOrBitsType<TB>
    auto operator&(TA a, TB b)
    {
        return static_cast<Flags>(a) & static_cast<Flags>(b);
    }

    template <typename TA, typename TB>
    requires IsFlagsOrBitsType<TA> && IsFlagsOrBitsType<TB>
    auto operator|(TA a, TB b)
    {
        return static_cast<Flags>(a) | static_cast<Flags>(b);
    }

    using BufferUsageFlags = Flags;
    enum class BufferUsageBits : BufferUsageFlags {
        MAP_READ      = 0x1,
        MAP_WRITE     = 0x2,
        COPY_SRC      = 0x4,
        COPY_DST      = 0x8,
        INDEX         = 0x10,
        VERTEX        = 0x20,
        UNIFORM       = 0x40,
        STORAGE       = 0x80,
        INDIRECT      = 0x100,
        QUERY_RESOLVE = 0x200,
        MAX
    };

    using TextureUsageFlags = Flags;
    enum class TextureUsageBits : TextureUsageFlags {
        COPY_SRC          = 0x1,
        COPY_DST          = 0x2,
        TEXTURE_BINDING   = 0x4,
        STORAGE_BINDING   = 0x8,
        RENDER_ATTACHMENT = 0x10,
        MAX
    };
}

namespace RHI {
    template <uint8_t N>
    struct Extent;

    template <>
    struct Extent<1> {
        size_t x;
    };

    template <>
    struct Extent<2> {
        size_t x;
        size_t y;
    };

    template <>
    struct Extent<3> {
        size_t x;
        size_t y;
        size_t z;
    };
}

#endif //EXPLOSION_RHI_ENUM_H
