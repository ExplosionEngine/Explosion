//
// Created by johnk on 10/1/2022.
//

#pragma once

#include <memory>
#include <cstdint>
#include <type_traits>

#include <Common/Memory.h>

namespace RHI {
    using EnumType = uint32_t;

    enum class RHIType : EnumType {
        DIRECTX_12,
        VULKAN,
        METAL,
        DUMMY,
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
        RGB9_E5_FLOAT,
        RGB10A2_UNORM,
        RG11B10_FLOAT,
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
        D16_UNORM,
        D24_UNORM_S8_UINT,
        D32_FLOAT,
        // TODO features / bc / etc / astc
        MAX
    };

    enum class VertexFormat : EnumType {
        // 8-Bits Channel
        UINT8_X2,
        UINT8_X4,
        SINT8_X2,
        SINT8_X4,
        UNORM8_X2,
        UNORM8_X4,
        SNORM8_X2,
        SNORM8_X4,
        // 16-Bits Channel
        UINT16_X2,
        UINT16_X4,
        SINT16_X2,
        SINT16_X4,
        UNORM16_X2,
        UNORM16_X4,
        SNORM16_X2,
        SNORM16_X4,
        FLOAT16_X2,
        FLOAT16_X4,
        // 32-Bits Channel
        FLOAT32_X1,
        FLOAT32_X2,
        FLOAT32_X3,
        FLOAT32_X4,
        UINT32_X1,
        UINT32_X2,
        UINT32_X3,
        UINT32_X4,
        SINT32_X1,
        SINT32_X2,
        SINT32_X3,
        SINT32_X4,
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
        COLOR,
        DEPTH,
        STENCIL,
        DEPTH_STENCIL,
        MAX
    };

    enum class AddressMode : EnumType {
        CLAMP_TO_EDGE,
        REPEAT,
        MIRROR_REPEAT,
        MAX
    };

    enum class FilterMode : EnumType {
        NEAREST,
        LINEAR,
        MAX
    };

    enum class ComparisonFunc : EnumType {
        NEVER,
        LESS,
        EQUAL,
        LESS_EQUAL,
        GREATER,
        NOT_EQUAL,
        GREATER_EQUAL,
        ALWAYS,
        MAX
    };

    enum class HlslBindingRangeType : EnumType {
        CONSTANT_BUFFER,
        TEXTURE,
        SAMPLER,
        UNORDERED_ACCESS,
        MAX
    };

    enum class BindingType : EnumType {
        UNIFORM_BUFFER,
        STORAGE_BUFFER,
        SAMPLER,
        TEXTURE,
        STORAGE_TEXTURE,
        MAX
    };

    enum class SamplerBindingType : EnumType {
        FILTERING,
        NON_FILTERING,
        COMPARISON,
        MAX
    };

    enum class TextureSampleType : EnumType {
        FLOAT,
        NON_FILTERABLE_FLOAT,
        DEPTH,
        SINT,
        UINT,
        MAX
    };

    enum class StorageTextureAccess : EnumType {
        WRITE_ONLY,
        MAX
    };

    enum class VertexStepMode : EnumType {
        PER_VERTEX,
        PER_INSTANCE,
        MAX
    };

    enum class PrimitiveTopologyType : EnumType {
        POINT,
        LINE,
        TRIANGLE,
        MAX
    };

    enum class PrimitiveTopology : EnumType {
        POINT_LIST,
        LINE_LIST,
        LINE_STRIP,
        TRIANGLE_LIST,
        TRIANGLE_STRIP,
        LINE_LIST_ADJ,
        LINE_STRIP_ADJ,
        TRIANGLE_LIST_ADJ,
        TRIANGLE_STRIP_ADJ,
        MAX
    };

    enum class IndexFormat : EnumType {
        UINT16,
        UINT32,
        MAX
    };

    enum class FrontFace : EnumType {
        CCW,
        CW,
        MAX
    };

    enum class CullMode : EnumType {
        NONE,
        FRONT,
        BACK,
        MAX
    };

    enum class StencilOp : EnumType {
        KEEP,
        ZERO,
        REPLACE,
        INVERT,
        INCREMENT_CLAMP,
        DECREMENT_CLAMP,
        INCREMENT_WRAP,
        DECREMENT_WRAP,
        MAX
    };

    enum class BlendFactor : EnumType {
        ZERO,
        ONE,
        SRC,
        ONE_MINUS_SRC,
        SRC_ALPHA,
        ONE_MINUS_SRC_ALPHA,
        DST,
        ONE_MINUS_DST,
        DST_ALPHA,
        ONE_MINUS_DST_ALPHA,
        // TODO check spec
        // SRC_ALPHA_SATURATED,
        // CONSTANT,
        // ONE_MINUS_CONSTANT,
        MAX
    };

    enum class BlendOp : EnumType {
        OP_ADD,
        OP_SUBTRACT,
        OP_REVERSE_SUBTRACT,
        OP_MIN,
        OP_MAX,
        MAX
    };

    enum class LoadOp : EnumType {
        LOAD,
        CLEAR,
        MAX
    };

    enum class StoreOp : EnumType {
        STORE,
        DISCARD,
        MAX
    };

    enum class PresentMode : EnumType {
        // TODO check this
        // 1. DirectX SwapEffect #see https://docs.microsoft.com/en-us/windows/win32/api/dxgi/ne-dxgi-dxgi_swap_effect
        // 2. Vulkan VkPresentModeKHR #see https://www.khronos.org/registry/vulkan/specs/1.3-extensions/man/html/VkPresentModeKHR.html
        IMMEDIATELY,
        VSYNC,
        MAX
    };

    enum class ResourceType : EnumType {
        BUFFER,
        TEXTURE,
        MAX
    };

    enum class BufferState : EnumType {
        UNDEFINED,
        COPY_SRC,
        COPY_DST,
        SHADER_READ_ONLY,
        STORAGE,
        MAX
    };

    enum class TextureState : EnumType {
        UNDEFINED,
        COPY_SRC,
        COPY_DST,
        SHADER_READ_ONLY,
        RENDER_TARGET,
        STORAGE,
        PRESENT,
        MAX
    };
}

#define RHI_FLAGS_DECLARE(FlagsType, BitsType) \
    FlagsType operator&(BitsType a, BitsType b); \
    FlagsType operator&(FlagsType a, BitsType b); \
    FlagsType operator|(BitsType a, BitsType b); \
    FlagsType operator|(FlagsType a, BitsType b); \

namespace RHI {
    template <typename T = uint32_t>
    class Flags {
    public:
        using UnderlyingType = T;

        Flags() = default;
        ~Flags() = default;
        Flags(T inValue) : value(inValue) {} // NOLINT

        template <typename E>
        requires std::is_same_v<T, std::underlying_type_t<E>>
        Flags(E e) : value(static_cast<T>(e)) {} // NOLINT

        T Value() const
        {
            return value;
        }

        explicit operator bool()
        {
            return value;
        }

        bool operator==(Flags other) const
        {
            return value == other.value;
        }

        bool operator!=(Flags other) const
        {
            return value != other.value;
        }

        bool operator==(T inValue) const
        {
            return value == inValue;
        }

        bool operator!=(T inValue) const
        {
            return value != inValue;
        }

        template <typename E>
        requires std::is_same_v<T, std::underlying_type_t<E>>
        bool operator==(E e) const
        {
            return value == static_cast<T>(e);
        }

        template <typename E>
        requires std::is_same_v<T, std::underlying_type_t<E>>
        bool operator!=(E e) const
        {
            return value != static_cast<T>(e);
        }

    private:
        T value;
    };

    template <typename T>
    Flags<T> operator&(Flags<T> a, Flags<T> b)
    {
        return Flags<T>(a.Value() & b.Value());
    }

    template <typename T>
    Flags<T> operator|(Flags<T> a, Flags<T> b)
    {
        return Flags<T>(a.Value() | b.Value());
    }

    using BufferUsageFlags = Flags<>;
    enum class BufferUsageBits : BufferUsageFlags::UnderlyingType {
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
    RHI_FLAGS_DECLARE(BufferUsageFlags, BufferUsageBits)

    using TextureUsageFlags = Flags<>;
    enum class TextureUsageBits : TextureUsageFlags::UnderlyingType {
        COPY_SRC                 = 0x1,
        COPY_DST                 = 0x2,
        TEXTURE_BINDING          = 0x4,
        STORAGE_BINDING          = 0x8,
        RENDER_ATTACHMENT        = 0x10,
        DEPTH_STENCIL_ATTACHMENT = 0x20,
        MAX
    };
    RHI_FLAGS_DECLARE(TextureUsageFlags, TextureUsageBits)

    using ShaderStageFlags = Flags<>;
    enum class ShaderStageBits : ShaderStageFlags::UnderlyingType {
        S_VERTEX   = 0x1,
        S_PIXEL    = 0x2,
        S_COMPUTE  = 0x4,
        S_GEOMETRY = 0x8,
        S_DOMAIN   = 0x10,
        S_HULL     = 0x20,
        MAX
    };
    RHI_FLAGS_DECLARE(ShaderStageFlags, ShaderStageBits)

    using ColorWriteFlags = Flags<>;
    enum class ColorWriteBits : ColorWriteFlags::UnderlyingType {
        RED   = 0x1,
        GREEN = 0x2,
        BLUE  = 0x4,
        ALPHA = 0x8,
        MAX
    };
    RHI_FLAGS_DECLARE(ColorWriteFlags, ColorWriteBits)
}

namespace std {
    template <typename T>
    struct hash<RHI::Flags<T>>
    {
        size_t operator()(RHI::Flags<T> flags) const
        {
            return hash<T>()(flags.Value());
        }
    };
}

namespace RHI {
    template <uint8_t N>
    struct Extent;

    template <>
    struct Extent<1> {
        uint32_t x;
    };

    template <>
    struct Extent<2> {
        uint32_t x;
        uint32_t y;
    };

    template <>
    struct Extent<3> {
        uint32_t x;
        uint32_t y;
        uint32_t z;
    };

    template <uint8_t N>
    struct ColorNormalized;

    template <>
    struct ColorNormalized<1> {
        float r;
    };

    template <>
    struct ColorNormalized<2> {
        float r;
        float g;
    };

    template <>
    struct ColorNormalized<3> {
        float r;
        float g;
        float b;
    };

    template <>
    struct ColorNormalized<4> {
        float r;
        float g;
        float b;
        float a;
    };
}
