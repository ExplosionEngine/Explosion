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
#include <Common/Math/Color.h>

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
    using EnumType = uint32_t;

    enum class RHIType : EnumType {
        directX12,
        vulkan,
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
        rwStorageBinding,
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

    enum class CompareFunc : EnumType {
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
        rwStorageBuffer,
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

    enum class FillMode : EnumType {
        wireframe,
        solid,
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
        rwStorage,
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
    using FlagBitsType = uint32_t;

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

    enum class BufferUsageBits : FlagBitsType {
        mapRead      = 0x1,
        mapWrite     = 0x2,
        copySrc      = 0x4,
        copyDst      = 0x8,
        index        = 0x10,
        vertex       = 0x20,
        uniform      = 0x40,
        storage      = 0x80,
        rwStorage    = 0x100,
        indirect     = 0x200,
        queryResolve = 0x400,
        max
    };
    using BufferUsageFlags = Flags<BufferUsageBits>;
    RHI_FLAGS_DECLARE(BufferUsageFlags, BufferUsageBits)

    enum class TextureUsageBits : FlagBitsType {
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

    enum class ShaderStageBits : FlagBitsType {
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

    enum class ColorWriteBits : FlagBitsType {
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

namespace std {
    template <typename E>
    struct hash<RHI::Flags<E>>
    {
        size_t operator()(RHI::Flags<E> flags) const
        {
            return hash<typename RHI::Flags<E>::UnderlyingType>()(flags.Value());
        }
    };
}