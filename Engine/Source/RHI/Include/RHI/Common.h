//
// Created by johnk on 10/1/2022.
//

#pragma once

#include <memory>
#include <cstdint>
#include <type_traits>

#include <Common/Memory.h>
#include <Common/String.h>
#include <Common/Math/Vector.h>
#include <Common/Math/Color.h>

namespace RHI {
    using EnumType = uint32_t;

    enum class RHIType : EnumType {
        directX12,
        vulkan,
        metal,
        dummy,
        max
    };

    enum class GpuType : EnumType {
        hardware,
        software,
        max
    };

    enum class QueueType : EnumType {
        graphics,
        compute,
        transfer,
        max
    };

    enum class MapMode : EnumType {
        read,
        write,
        max
    };

    enum class PixelFormat : EnumType {
        // 8-Bits
        r8Unorm,
        r8Snorm,
        r8Uint,
        r8Sint,
        // 16-Bits
        r16Uint,
        r16Sint,
        r16Float,
        rg8Unorm,
        rg8Snorm,
        rg8Uint,
        rg8Sint,
        // 32-Bits
        r32Uint,
        r32Sint,
        r32Float,
        rg16Uint,
        rg16Sint,
        rg16Float,
        rgba8Unorm,
        rgba8UnormSrgb,
        rgba8Snorm,
        rgba8Uint,
        rgba8Sint,
        bgra8Unorm,
        bgra8UnormSrgb,
        rgb9E5Float,
        rgb10A2Unorm,
        rg11B10Float,
        // 64-Bits
        rg32Uint,
        rg32Sint,
        rg32Float,
        rgba16Uint,
        rgba16Sint,
        rgba16Float,
        // 128-Bits
        rgba32Uint,
        rgba32Sint,
        rgba32Float,
        // Depth-Stencil
        d16Unorm,
        d24UnormS8Uint,
        d32Float,
        d32FloatS8Uint,
        // TODO features / bc / etc / astc
        max
    };

    enum class VertexFormat : EnumType {
        // 8-Bits Channel
        uint8X2,
        uint8X4,
        sint8X2,
        sint8X4,
        unorm8X2,
        unorm8X4,
        snorm8X2,
        snorm8X4,
        // 16-Bits Channel
        uint16X2,
        uint16X4,
        sint16X2,
        sint16X4,
        unorm16X2,
        unorm16X4,
        snorm16X2,
        snorm16X4,
        float16X2,
        float16X4,
        // 32-Bits Channel
        float32X1,
        float32X2,
        float32X3,
        float32X4,
        uint32X1,
        uint32X2,
        uint32X3,
        uint32X4,
        sint32X1,
        sint32X2,
        sint32X3,
        sint32X4,
        max
    };

    enum class TextureDimension : EnumType {
        t1D,
        t2D,
        t3D,
        max
    };

    enum class TextureViewDimension : EnumType {
        tv1D,
        tv2D,
        tv2DArray,
        tvCube,
        tvCubeArray,
        tv3D,
        max
    };

    enum class TextureAspect : EnumType {
        color,
        depth,
        stencil,
        depthStencil,
        max
    };

    enum class TextureViewType : EnumType {
        textureBinding,
        storageBinding,
        colorAttachment,
        depthStencil,
        max
    };

    enum class BufferViewType : EnumType {
        vertex,
        index,
        uniformBinding,
        storageBinding,
        max
    };

    enum class AddressMode : EnumType {
        clampToEdge,
        repeat,
        mirrorRepeat,
        max
    };

    enum class FilterMode : EnumType {
        nearest,
        linear,
        max
    };

    enum class ComparisonFunc : EnumType {
        never,
        less,
        equal,
        lessEqual,
        greater,
        notEqual,
        greaterEqual,
        always,
        max
    };

    enum class HlslBindingRangeType : EnumType {
        constantBuffer,
        texture,
        sampler,
        unorderedAccess,
        max
    };

    enum class BindingType : EnumType {
        uniformBuffer,
        storageBuffer,
        sampler,
        texture,
        storageTexture,
        max
    };

    enum class SamplerBindingType : EnumType {
        filtering,
        nonFiltering,
        comparison,
        max
    };

    enum class TextureSampleType : EnumType {
        filterableFloat,
        nonFilterableFloat,
        depth,
        sint,
        uint,
        max
    };

    enum class StorageTextureAccess : EnumType {
        writeOnly,
        max
    };

    enum class VertexStepMode : EnumType {
        perVertex,
        perInstance,
        max
    };

    enum class PrimitiveTopologyType : EnumType {
        point,
        line,
        triangle,
        max
    };

    enum class PrimitiveTopology : EnumType {
        pointList,
        lineList,
        lineStrip,
        triangleList,
        triangleStrip,
        lineListAdj,
        lineStripAdj,
        triangleListAdj,
        triangleStripAdj,
        max
    };

    enum class IndexFormat : EnumType {
        uint16,
        uint32,
        max
    };

    enum class FrontFace : EnumType {
        ccw,
        cw,
        max
    };

    enum class CullMode : EnumType {
        none,
        front,
        back,
        max
    };

    enum class StencilOp : EnumType {
        keep,
        zero,
        replace,
        invert,
        incrementClamp,
        decrementClamp,
        incrementWrap,
        decrementWrap,
        max
    };

    enum class BlendFactor : EnumType {
        zero,
        one,
        src,
        oneMinusSrc,
        srcAlpha,
        oneMinusSrcAlpha,
        dst,
        oneMinusDst,
        dstAlpha,
        oneMinusDstAlpha,
        // TODO check spec
        // scrAlphaSaturated,
        // constant,
        // oneMinusConstant,
        max
    };

    enum class BlendOp : EnumType {
        opAdd,
        opSubstract,
        opReverseSubstract,
        opMin,
        opMax,
        max
    };

    enum class LoadOp : EnumType {
        load,
        clear,
        max
    };

    enum class StoreOp : EnumType {
        store,
        discard,
        max
    };

    enum class PresentMode : EnumType {
        // TODO check this
        // 1. DirectX SwapEffect #see https://docs.microsoft.com/en-us/windows/win32/api/dxgi/ne-dxgi-dxgi_swap_effect
        // 2. Vulkan VkPresentModeKHR #see https://www.khronos.org/registry/vulkan/specs/1.3-extensions/man/html/VkPresentModeKHR.html
        immediately,
        vsync,
        max
    };

    enum class ResourceType : EnumType {
        buffer,
        texture,
        max
    };

    enum class BufferState : EnumType {
        undefined,
        staging,
        copySrc,
        copyDst,
        shaderReadOnly,
        storage,
        max
    };

    enum class TextureState : EnumType {
        undefined,
        copySrc,
        copyDst,
        shaderReadOnly,
        renderTarget,
        storage,
        depthStencilReadonly,
        depthStencilWrite,
        present,
        max
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
        static Flags null;

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
    Flags<T> Flags<T>::null = Flags<T>(0);

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
        mapRead      = 0x1,
        mapWrite     = 0x2,
        copySrc      = 0x4,
        copyDst      = 0x8,
        index        = 0x10,
        vertex       = 0x20,
        uniform      = 0x40,
        storage      = 0x80,
        indirect     = 0x100,
        queryResolve = 0x200,
        max
    };
    RHI_FLAGS_DECLARE(BufferUsageFlags, BufferUsageBits)

    using TextureUsageFlags = Flags<>;
    enum class TextureUsageBits : TextureUsageFlags::UnderlyingType {
        copySrc                 = 0x1,
        copyDst                 = 0x2,
        textureBinding          = 0x4,
        storageBinding          = 0x8,
        renderAttachment        = 0x10,
        depthStencilAttachment  = 0x20,
        max
    };
    RHI_FLAGS_DECLARE(TextureUsageFlags, TextureUsageBits)

    using ShaderStageFlags = Flags<>;
    enum class ShaderStageBits : ShaderStageFlags::UnderlyingType {
        sVertex   = 0x1,
        sPixel    = 0x2,
        sCompute  = 0x4,
        sGeometry = 0x8,
        sHull     = 0x10,
        sDomain   = 0x20,
        max
    };
    RHI_FLAGS_DECLARE(ShaderStageFlags, ShaderStageBits)

    using ColorWriteFlags = Flags<>;
    enum class ColorWriteBits : ColorWriteFlags::UnderlyingType {
        red   = 0x1,
        green = 0x2,
        blue  = 0x4,
        alpha = 0x8,
        max
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