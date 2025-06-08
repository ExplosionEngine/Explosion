//
// Created by johnk on 10/1/2022.
//

#pragma once

#include <cstdint>
#include <type_traits>

#include <Common/Utility.h>
#include <Common/Memory.h>
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

#define ALIGN_AS_GPU alignas(16)

namespace RHI {
    enum class RHIType : uint8_t {
        directX12,
        vulkan,
        dummy,
        max
    };

    enum class GpuType : uint8_t {
        hardware,
        software,
        max
    };

    enum class QueueType : uint8_t {
        graphics,
        compute,
        transfer,
        max
    };

    enum class MapMode : uint8_t {
        read,
        write,
        max
    };

    enum class PixelFormat : uint8_t {
        // 8-Bits
        begin8Bits,
        r8Unorm,
        r8Snorm,
        r8Uint,
        r8Sint,
        // 16-Bits
        begin16Bits,
        r16Uint,
        r16Sint,
        r16Float,
        rg8Unorm,
        rg8Snorm,
        rg8Uint,
        rg8Sint,
        d16Unorm,
        // 32-Bits
        begin32Bits,
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
        d24UnormS8Uint,
        d32Float,
        // 64-Bits
        begin64Bits,
        rg32Uint,
        rg32Sint,
        rg32Float,
        rgba16Uint,
        rgba16Sint,
        rgba16Float,
        d32FloatS8Uint,
        // 128-Bits
        begin128Bits,
        rgba32Uint,
        rgba32Sint,
        rgba32Float,
        max
    };

    enum class VertexFormat : uint8_t {
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

    enum class TextureDimension : uint8_t {
        t1D,
        t2D,
        t3D,
        max
    };

    enum class TextureViewDimension : uint8_t {
        tv1D,
        tv2D,
        tv2DArray,
        tvCube,
        tvCubeArray,
        tv3D,
        max
    };

    enum class TextureAspect : uint8_t {
        color,
        depth,
        stencil,
        depthStencil,
        max
    };

    enum class TextureViewType : uint8_t {
        textureBinding,
        storageBinding,
        colorAttachment,
        depthStencil,
        max
    };

    enum class BufferViewType : uint8_t {
        vertex,
        index,
        uniformBinding,
        storageBinding,
        rwStorageBinding,
        max
    };

    enum class AddressMode : uint8_t {
        clampToEdge,
        repeat,
        mirrorRepeat,
        max
    };

    enum class FilterMode : uint8_t {
        nearest,
        linear,
        max
    };

    enum class CompareFunc : uint8_t {
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

    enum class HlslBindingRangeType : uint8_t {
        constantBuffer,
        texture,
        sampler,
        unorderedAccess,
        max
    };

    enum class BindingType : uint8_t {
        uniformBuffer,
        storageBuffer,
        rwStorageBuffer,
        sampler,
        texture,
        storageTexture,
        max
    };

    enum class SamplerBindingType : uint8_t {
        filtering,
        nonFiltering,
        comparison,
        max
    };

    enum class TextureSampleType : uint8_t {
        filterableFloat,
        nonFilterableFloat,
        depth,
        sint,
        uint,
        max
    };

    enum class StorageTextureAccess : uint8_t {
        writeOnly,
        max
    };

    enum class VertexStepMode : uint8_t {
        perVertex,
        perInstance,
        max
    };

    enum class PrimitiveTopologyType : uint8_t {
        point,
        line,
        triangle,
        max
    };

    enum class PrimitiveTopology : uint8_t {
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

    enum class IndexFormat : uint8_t {
        uint16,
        uint32,
        max
    };

    enum class FrontFace : uint8_t {
        ccw,
        cw,
        max
    };

    enum class FillMode : uint8_t {
        wireframe,
        solid,
        max
    };

    enum class CullMode : uint8_t {
        none,
        front,
        back,
        max
    };

    enum class StencilOp : uint8_t {
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

    enum class BlendFactor : uint8_t {
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

    enum class BlendOp : uint8_t {
        opAdd,
        opSubstract,
        opReverseSubstract,
        opMin,
        opMax,
        max
    };

    enum class LoadOp : uint8_t {
        load,
        clear,
        max
    };

    enum class StoreOp : uint8_t {
        store,
        discard,
        max
    };

    enum class PresentMode : uint8_t {
        // TODO check this
        // 1. DirectX SwapEffect #see https://docs.microsoft.com/en-us/windows/win32/api/dxgi/ne-dxgi-dxgi_swap_effect
        // 2. Vulkan VkPresentModeKHR #see https://www.khronos.org/registry/vulkan/specs/1.3-extensions/man/html/VkPresentModeKHR.html
        immediately,
        vsync,
        max
    };

    enum class ResourceType : uint8_t {
        buffer,
        texture,
        max
    };

    enum class BufferState : uint8_t {
        undefined,
        staging,
        copySrc,
        copyDst,
        shaderReadOnly,
        storage,
        rwStorage,
        max
    };

    enum class TextureState : uint8_t {
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

namespace RHI {
    enum class BufferUsageBits : uint16_t {
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
        max          = 0x800
    };
    using BufferUsageFlags = Common::Flags<BufferUsageBits>;
    DECLARE_FLAG_BITS_OP(BufferUsageFlags, BufferUsageBits)

    enum class TextureUsageBits : uint8_t {
        copySrc                 = 0x1,
        copyDst                 = 0x2,
        textureBinding          = 0x4,
        storageBinding          = 0x8,
        renderAttachment        = 0x10,
        depthStencilAttachment  = 0x20,
        max                     = 0x40
    };
    using TextureUsageFlags = Common::Flags<TextureUsageBits>;
    DECLARE_FLAG_BITS_OP(TextureUsageFlags, TextureUsageBits)

    enum class ShaderStageBits : uint8_t {
        sVertex   = 0x1,
        sPixel    = 0x2,
        sCompute  = 0x4,
        sGeometry = 0x8,
        sHull     = 0x10,
        sDomain   = 0x20,
        max       = 0x40,
    };
    using ShaderStageFlags = Common::Flags<ShaderStageBits>;
    DECLARE_FLAG_BITS_OP(ShaderStageFlags, ShaderStageBits)

    enum class ColorWriteBits : uint8_t {
        red   = 0x1,
        green = 0x2,
        blue  = 0x4,
        alpha = 0x8,
        max   = 0x10,
        rgb   = red | green | blue,
        all   = red | green | blue | alpha
    };
    using ColorWriteFlags = Common::Flags<ColorWriteBits>;
    DECLARE_FLAG_BITS_OP(ColorWriteFlags, ColorWriteBits)
}

namespace RHI {
    size_t GetBytesPerPixel(PixelFormat format);
}