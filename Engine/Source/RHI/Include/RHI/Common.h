//
// Created by johnk on 10/1/2022.
//

#pragma once

#include <memory>
#include <cstdint>
#include <type_traits>
#include <functional>

#include <Common/Memory.h>
#include <Common/String.h>
#include <Common/Math/Vector.h>

#define DECLARE_EC_FUNC() template <typename A, typename B> inline B EnumCast(const A& value);
#define ECIMPL_BEGIN(A, B) template <> inline B EnumCast<A, B>(const A& value) {
#define ECIMPL_ITEM(A, B) if (value == A) { return B; }
#define ECIMPL_END(B) Unimplement(); return (B) 0; };

#define DECLARE_FC_FUNC() template <typename A, typename B> inline B FlagsCast(const A& flags);
#define FCIMPL_BEGIN(A, B) template <> inline B FlagsCast<A, B>(const A& flags) { B result = (B) 0;
#define FCIMPL_ITEM(A, B) if (flags & A) { result |= B; }
#define FCIMPL_END(B) return result; };

namespace RHI {
    template <typename E>
    using BitsTypeForEachFunc = std::function<void(E e)>;

    template <typename E>
    void ForEachBitsType(BitsTypeForEachFunc<E>&& func)
    {
        using UBitsType = std::underlying_type_t<E>;
        for (UBitsType i = 0x1; i < static_cast<UBitsType>(E::max); i = i << 1) {
            func(static_cast<E>(i));
        }
    }
}

namespace RHI {
    using EnumUint8T = uint8_t;
    using EnumUint16T = uint16_t;
    using EnumUint32T = uint32_t;
    using EnumUint64T = uint64_t;

    enum class RHIType : EnumUint8T {
        directX12,
        vulkan,
        dummy,
        max
    };

    enum class GpuType : EnumUint8T {
        hardware,
        software,
        max
    };

    enum class QueueType : EnumUint8T {
        graphics,
        compute,
        transfer,
        max
    };

    enum class MapMode : EnumUint8T {
        read,
        write,
        max
    };

    enum class PixelFormat : EnumUint8T {
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
        max
    };

    enum class VertexFormat : EnumUint8T {
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

    enum class TextureDimension : EnumUint8T {
        t1D,
        t2D,
        t3D,
        max
    };

    enum class TextureViewDimension : EnumUint8T {
        tv1D,
        tv2D,
        tv2DArray,
        tvCube,
        tvCubeArray,
        tv3D,
        max
    };

    enum class TextureAspect : EnumUint8T {
        color,
        depth,
        stencil,
        depthStencil,
        max
    };

    enum class TextureViewType : EnumUint8T {
        textureBinding,
        storageBinding,
        colorAttachment,
        depthStencil,
        max
    };

    enum class BufferViewType : EnumUint8T {
        vertex,
        index,
        uniformBinding,
        storageBinding,
        max
    };

    enum class AddressMode : EnumUint8T {
        clampToEdge,
        repeat,
        mirrorRepeat,
        max
    };

    enum class FilterMode : EnumUint8T {
        nearest,
        linear,
        max
    };

    enum class CompareFunc : EnumUint8T {
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

    enum class HlslBindingRangeType : EnumUint8T {
        constantBuffer,
        texture,
        sampler,
        unorderedAccess,
        max
    };

    enum class BindingType : EnumUint8T {
        uniformBuffer,
        storageBuffer,
        sampler,
        texture,
        storageTexture,
        max
    };

    enum class SamplerBindingType : EnumUint8T {
        filtering,
        nonFiltering,
        comparison,
        max
    };

    enum class TextureSampleType : EnumUint8T {
        filterableFloat,
        nonFilterableFloat,
        depth,
        sint,
        uint,
        max
    };

    enum class StorageTextureAccess : EnumUint8T {
        writeOnly,
        max
    };

    enum class VertexStepMode : EnumUint8T {
        perVertex,
        perInstance,
        max
    };

    enum class PrimitiveTopologyType : EnumUint8T {
        point,
        line,
        triangle,
        max
    };

    enum class PrimitiveTopology : EnumUint8T {
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

    enum class IndexFormat : EnumUint8T {
        uint16,
        uint32,
        max
    };

    // TODO: Support more format
    enum class StorageFormat: EnumUint8T {
        float32,
        uint32,
        sint32,
        max
    };

    enum class FrontFace : EnumUint8T {
        ccw,
        cw,
        max
    };

    enum class FillMode : EnumUint8T {
        wireframe,
        solid,
        max
    };

    enum class CullMode : EnumUint8T {
        none,
        front,
        back,
        max
    };

    enum class StencilOp : EnumUint8T {
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

    enum class BlendFactor : EnumUint8T {
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
        max
    };

    enum class BlendOp : EnumUint8T {
        opAdd,
        opSubstract,
        opReverseSubstract,
        opMin,
        opMax,
        max
    };

    enum class LoadOp : EnumUint8T {
        load,
        clear,
        max
    };

    enum class StoreOp : EnumUint8T {
        store,
        discard,
        max
    };

    enum class PresentMode : EnumUint8T {
        // TODO check this
        // 1. DirectX SwapEffect #see https://docs.microsoft.com/en-us/windows/win32/api/dxgi/ne-dxgi-dxgi_swap_effect
        // 2. Vulkan VkPresentModeKHR #see https://www.khronos.org/registry/vulkan/specs/1.3-extensions/man/html/VkPresentModeKHR.html
        immediately,
        vsync,
        max
    };

    enum class ResourceType : EnumUint8T {
        buffer,
        texture,
        max
    };

    enum class BufferState : EnumUint8T {
        undefined,
        staging,  // TODO: the sementics of staging is intermediate / temporary, change to `coherent` maybe, used for host upload and readback
        copySrc,
        copyDst,
        shaderReadOnly,
        storage,
        max
    };

    enum class TextureState : EnumUint8T {
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
    using FlagBitsUint8T = uint8_t;
    using FlagBitsUint16T = uint16_t;
    using FlagBitsUint32T = uint32_t;
    using FlagBitsUint64T = uint64_t;

    template <typename E>
    class Flags {
    public:
        static Flags null;

        using UnderlyingType = std::underlying_type_t<E>;

        Flags() = default;
        ~Flags() = default;
        Flags(UnderlyingType inValue) : value(inValue) {} // NOLINT
        Flags(E e) : value(static_cast<UnderlyingType>(e)) {} // NOLINT

        UnderlyingType Value() const
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

        bool operator==(UnderlyingType inValue) const
        {
            return value == inValue;
        }

        bool operator!=(UnderlyingType inValue) const
        {
            return value != inValue;
        }

        bool operator==(E e) const
        {
            return value == static_cast<UnderlyingType>(e);
        }

        bool operator!=(E e) const
        {
            return value != static_cast<UnderlyingType>(e);
        }

    private:
        UnderlyingType value;
    };

    template <typename E>
    Flags<E> Flags<E>::null = Flags<E>(0);

    template <typename E>
    Flags<E> operator&(Flags<E> a, Flags<E> b)
    {
        return Flags<E>(a.Value() & b.Value());
    }

    template <typename E>
    Flags<E> operator|(Flags<E> a, Flags<E> b)
    {
        return Flags<E>(a.Value() | b.Value());
    }

    enum class BufferUsageBits : FlagBitsUint16T {
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
    using BufferUsageFlags = Flags<BufferUsageBits>;
    RHI_FLAGS_DECLARE(BufferUsageFlags, BufferUsageBits)

    enum class TextureUsageBits : FlagBitsUint8T {
        copySrc                 = 0x1,
        copyDst                 = 0x2,
        textureBinding          = 0x4,
        storageBinding          = 0x8,
        renderAttachment        = 0x10,
        depthStencilAttachment  = 0x20,
        max
    };
    using TextureUsageFlags = Flags<TextureUsageBits>;
    RHI_FLAGS_DECLARE(TextureUsageFlags, TextureUsageBits)

    enum class ShaderStageBits : FlagBitsUint8T {
        sVertex   = 0x1,
        sPixel    = 0x2,
        sCompute  = 0x4,
        sGeometry = 0x8,
        sHull     = 0x10,
        sDomain   = 0x20,
        max
    };
    using ShaderStageFlags = Flags<ShaderStageBits>;
    RHI_FLAGS_DECLARE(ShaderStageFlags, ShaderStageBits)

    enum class ColorWriteBits : FlagBitsUint8T {
        red   = 0x1,
        green = 0x2,
        blue  = 0x4,
        alpha = 0x8,
        max,
        rgb   = red | green | blue,
        all   = red | green | blue | alpha
    };
    using ColorWriteFlags = Flags<ColorWriteBits>;
    RHI_FLAGS_DECLARE(ColorWriteFlags, ColorWriteBits)
}

namespace std { // NOLINT
    template <typename E>
    struct hash<RHI::Flags<E>>
    {
        size_t operator()(RHI::Flags<E> flags) const
        {
            return hash<typename RHI::Flags<E>::UnderlyingType>()(flags.Value());
        }
    };
}