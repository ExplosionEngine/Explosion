//
// Created by Zach Lee on 2022/10/28.
//


#pragma once

#include <Common/Debug.h>
#include <RHI/Common.h>
#include <unordered_map>
#import <Metal/Metal.h>

// duplicated code because static variable and namespace
namespace RHI::Metal {
    template <typename A, typename B>
    static const std::unordered_map<A, B> MTL_ENUM_MAP;

    template <typename A, typename B>
    B MTLEnumCast(const A& value)
    {
        auto iter = MTL_ENUM_MAP<A, B>.find(value);
        Assert((iter != MTL_ENUM_MAP<A, B>.end()));
        return static_cast<B>(iter->second);
    }

#define MTL_ENUM_MAP_BEGIN(A, B) template <> static const std::unordered_map<A, B> MTL_ENUM_MAP<A, B> = {
#define MTL_ENUM_MAP_ITEM(A, B) { A, B },
#define MTL_ENUM_MAP_END() };
}

// enum map definitions
namespace RHI::Metal {
    MTL_ENUM_MAP_BEGIN(PixelFormat, MTLPixelFormat)
    // 8-Bits
    MTL_ENUM_MAP_ITEM(PixelFormat::r8Unorm, MTLPixelFormatR8Unorm)
    MTL_ENUM_MAP_ITEM(PixelFormat::r8Snorm, MTLPixelFormatR8Snorm)
    MTL_ENUM_MAP_ITEM(PixelFormat::r8Uint,  MTLPixelFormatR8Uint)
    MTL_ENUM_MAP_ITEM(PixelFormat::r8Sint,  MTLPixelFormatR8Sint)
    // 16-Bits
    MTL_ENUM_MAP_ITEM(PixelFormat::r16Uint,  MTLPixelFormatR16Uint)
    MTL_ENUM_MAP_ITEM(PixelFormat::r16Sint,  MTLPixelFormatR16Sint)
    MTL_ENUM_MAP_ITEM(PixelFormat::r16Float, MTLPixelFormatR16Float)
    MTL_ENUM_MAP_ITEM(PixelFormat::rg8Unorm, MTLPixelFormatRG8Unorm)
    MTL_ENUM_MAP_ITEM(PixelFormat::rg8Snorm, MTLPixelFormatRG8Snorm)
    MTL_ENUM_MAP_ITEM(PixelFormat::rg8Uint,  MTLPixelFormatRG8Uint)
    MTL_ENUM_MAP_ITEM(PixelFormat::rg8Sint,  MTLPixelFormatRG8Sint)
    // 32-Bits
    MTL_ENUM_MAP_ITEM(PixelFormat::r32Uint,         MTLPixelFormatR32Uint)
    MTL_ENUM_MAP_ITEM(PixelFormat::r32Sint,         MTLPixelFormatR32Sint)
    MTL_ENUM_MAP_ITEM(PixelFormat::r32Float,        MTLPixelFormatR32Float)
    MTL_ENUM_MAP_ITEM(PixelFormat::rg16Uint,        MTLPixelFormatRG16Uint)
    MTL_ENUM_MAP_ITEM(PixelFormat::rg16Sint,        MTLPixelFormatRG16Sint)
    MTL_ENUM_MAP_ITEM(PixelFormat::rg16Float,       MTLPixelFormatRG16Float)
    MTL_ENUM_MAP_ITEM(PixelFormat::rgba8Unorm,      MTLPixelFormatRGBA8Unorm)
    MTL_ENUM_MAP_ITEM(PixelFormat::rgba8UnormSrgb, MTLPixelFormatRGBA8Unorm_sRGB)
    MTL_ENUM_MAP_ITEM(PixelFormat::rgba8Snorm,      MTLPixelFormatRGBA8Snorm)
    MTL_ENUM_MAP_ITEM(PixelFormat::rgba8Uint,       MTLPixelFormatRGBA8Uint)
    MTL_ENUM_MAP_ITEM(PixelFormat::rgba8Sint,       MTLPixelFormatRGBA8Sint)
    MTL_ENUM_MAP_ITEM(PixelFormat::bgra8Unorm,      MTLPixelFormatBGRA8Unorm)
    MTL_ENUM_MAP_ITEM(PixelFormat::bgra8UnormSrgb, MTLPixelFormatBGRA8Unorm_sRGB)
    MTL_ENUM_MAP_ITEM(PixelFormat::rgb9E5Float,    MTLPixelFormatRGB9E5Float)
    MTL_ENUM_MAP_ITEM(PixelFormat::rgb10A2Unorm,    MTLPixelFormatRGB10A2Unorm)
    MTL_ENUM_MAP_ITEM(PixelFormat::rg11B10Float,    MTLPixelFormatRG11B10Float)
    // 64-Bits
    MTL_ENUM_MAP_ITEM(PixelFormat::rg32Uint,        MTLPixelFormatRG32Uint)
    MTL_ENUM_MAP_ITEM(PixelFormat::rg32Sint,        MTLPixelFormatRG32Sint)
    MTL_ENUM_MAP_ITEM(PixelFormat::rg32Float,       MTLPixelFormatRG32Float)
    MTL_ENUM_MAP_ITEM(PixelFormat::rgba16Uint,      MTLPixelFormatRGBA16Uint)
    MTL_ENUM_MAP_ITEM(PixelFormat::rgba16Sint,      MTLPixelFormatRGBA16Sint)
    MTL_ENUM_MAP_ITEM(PixelFormat::rgba16Float,     MTLPixelFormatRGBA16Float)
    // 128-Bits
    MTL_ENUM_MAP_ITEM(PixelFormat::rgba32Uint,      MTLPixelFormatRGBA32Uint)
    MTL_ENUM_MAP_ITEM(PixelFormat::rgba32Sint,      MTLPixelFormatRGBA32Sint)
    MTL_ENUM_MAP_ITEM(PixelFormat::rgba32Float,     MTLPixelFormatRGBA32Float)
    // Depth-Stencil
    MTL_ENUM_MAP_ITEM(PixelFormat::d16Unorm,         MTLPixelFormatDepth16Unorm)
    MTL_ENUM_MAP_ITEM(PixelFormat::d24UnormS8Uint, MTLPixelFormatDepth24Unorm_Stencil8)
    MTL_ENUM_MAP_ITEM(PixelFormat::d32Float,         MTLPixelFormatDepth32Float)
    // Undefined
    MTL_ENUM_MAP_ITEM(PixelFormat::max,               MTLPixelFormatInvalid)
    MTL_ENUM_MAP_END()

//    VK_ENUM_MAP_BEGIN(PrimitiveTopologyType, vk::PrimitiveTopology)
//    VK_ENUM_MAP_ITEM(PrimitiveTopologyType::point, vk::PrimitiveTopology::ePointList)
//    VK_ENUM_MAP_ITEM(PrimitiveTopologyType::line, vk::PrimitiveTopology::eLineList)
//    VK_ENUM_MAP_ITEM(PrimitiveTopologyType::triangle, vk::PrimitiveTopology::eTriangleList)
//    VK_ENUM_MAP_END()
//
//    VK_ENUM_MAP_BEGIN(CullMode, vk::CullModeFlagBits)
//    VK_ENUM_MAP_ITEM(CullMode::none, vk::CullModeFlagBits::eNone)
//    VK_ENUM_MAP_ITEM(CullMode::front, vk::CullModeFlagBits::eFront)
//    VK_ENUM_MAP_ITEM(CullMode::back, vk::CullModeFlagBits::eBack)
//    VK_ENUM_MAP_END()
//
//    VK_ENUM_MAP_BEGIN(BlendOp, vk::BlendOp)
//    VK_ENUM_MAP_ITEM(BlendOp::opAdd,              vk::BlendOp::eAdd)
//    VK_ENUM_MAP_ITEM(BlendOp::opSubstract,         vk::BlendOp::eSubtract)
//    VK_ENUM_MAP_ITEM(BlendOp::opReverseSubstract, vk::BlendOp::eReverseSubtract)
//    VK_ENUM_MAP_ITEM(BlendOp::opMin,              vk::BlendOp::eMin)
//    VK_ENUM_MAP_ITEM(BlendOp::opMax,              vk::BlendOp::eMax)
//    VK_ENUM_MAP_END()
//
//    VK_ENUM_MAP_BEGIN(BlendFactor, vk::BlendFactor)
//    VK_ENUM_MAP_ITEM(BlendFactor::zero,                vk::BlendFactor::eZero)
//    VK_ENUM_MAP_ITEM(BlendFactor::one,                 vk::BlendFactor::eOne)
//    VK_ENUM_MAP_ITEM(BlendFactor::src,                 vk::BlendFactor::eSrcColor)
//    VK_ENUM_MAP_ITEM(BlendFactor::oneMinusSrc,       vk::BlendFactor::eOneMinusSrcColor)
//    VK_ENUM_MAP_ITEM(BlendFactor::srcAlpha,           vk::BlendFactor::eSrcAlpha)
//    VK_ENUM_MAP_ITEM(BlendFactor::oneMinusSrcAlpha, vk::BlendFactor::eOneMinusSrcAlpha)
//    VK_ENUM_MAP_ITEM(BlendFactor::dst,                 vk::BlendFactor::eDstColor)
//    VK_ENUM_MAP_ITEM(BlendFactor::oneMinusDst,       vk::BlendFactor::eOneMinusDstColor)
//    VK_ENUM_MAP_ITEM(BlendFactor::dstAlpha,           vk::BlendFactor::eDstAlpha)
//    VK_ENUM_MAP_ITEM(BlendFactor::oneMinusDstAlpha, vk::BlendFactor::eOneMinusDstAlpha)
//    VK_ENUM_MAP_END()
//
    MTL_ENUM_MAP_BEGIN(VertexFormat, MTLVertexFormat)
    MTL_ENUM_MAP_ITEM(VertexFormat::uint8X2,   MTLVertexFormatUChar2)
    MTL_ENUM_MAP_ITEM(VertexFormat::uint8X4,   MTLVertexFormatUChar4)
    MTL_ENUM_MAP_ITEM(VertexFormat::sint8X2,   MTLVertexFormatChar2)
    MTL_ENUM_MAP_ITEM(VertexFormat::sint8X4,   MTLVertexFormatChar4)
    MTL_ENUM_MAP_ITEM(VertexFormat::unorm8X2,  MTLVertexFormatUChar2Normalized)
    MTL_ENUM_MAP_ITEM(VertexFormat::unorm8X4,  MTLVertexFormatUChar4Normalized)
    MTL_ENUM_MAP_ITEM(VertexFormat::snorm8X2,  MTLVertexFormatChar2Normalized)
    MTL_ENUM_MAP_ITEM(VertexFormat::snorm8X4,  MTLVertexFormatChar4Normalized)
    MTL_ENUM_MAP_ITEM(VertexFormat::uint16X2,  MTLVertexFormatUShort2)
    MTL_ENUM_MAP_ITEM(VertexFormat::uint16X4,  MTLVertexFormatUShort4)
    MTL_ENUM_MAP_ITEM(VertexFormat::sint16X2,  MTLVertexFormatShort2)
    MTL_ENUM_MAP_ITEM(VertexFormat::sint16X4,  MTLVertexFormatShort4)
    MTL_ENUM_MAP_ITEM(VertexFormat::unorm16X2, MTLVertexFormatUShort2Normalized)
    MTL_ENUM_MAP_ITEM(VertexFormat::unorm16X4, MTLVertexFormatUShort4Normalized)
    MTL_ENUM_MAP_ITEM(VertexFormat::snorm16X2, MTLVertexFormatShort2Normalized)
    MTL_ENUM_MAP_ITEM(VertexFormat::snorm16X4, MTLVertexFormatShort4Normalized)
    MTL_ENUM_MAP_ITEM(VertexFormat::float16X2, MTLVertexFormatHalf2)
    MTL_ENUM_MAP_ITEM(VertexFormat::float16X4, MTLVertexFormatHalf4)
    MTL_ENUM_MAP_ITEM(VertexFormat::float32X1, MTLVertexFormatFloat)
    MTL_ENUM_MAP_ITEM(VertexFormat::float32X2, MTLVertexFormatFloat2)
    MTL_ENUM_MAP_ITEM(VertexFormat::float32X3, MTLVertexFormatFloat3)
    MTL_ENUM_MAP_ITEM(VertexFormat::float32X4, MTLVertexFormatFloat4)
    MTL_ENUM_MAP_ITEM(VertexFormat::uint32X1,  MTLVertexFormatUInt)
    MTL_ENUM_MAP_ITEM(VertexFormat::uint32X2,  MTLVertexFormatUInt2)
    MTL_ENUM_MAP_ITEM(VertexFormat::uint32X3,  MTLVertexFormatUInt3)
    MTL_ENUM_MAP_ITEM(VertexFormat::uint32X4,  MTLVertexFormatUInt4)
    MTL_ENUM_MAP_ITEM(VertexFormat::sint32X1,  MTLVertexFormatInt)
    MTL_ENUM_MAP_ITEM(VertexFormat::sint32X2,  MTLVertexFormatInt2)
    MTL_ENUM_MAP_ITEM(VertexFormat::sint32X3,  MTLVertexFormatInt3)
    MTL_ENUM_MAP_ITEM(VertexFormat::sint32X4,  MTLVertexFormatInt4)
    MTL_ENUM_MAP_END()
//
//    VK_ENUM_MAP_BEGIN(BindingType, vk::DescriptorType)
//    VK_ENUM_MAP_ITEM(BindingType::uniformBuffer,  vk::DescriptorType::eUniformBuffer)
//    VK_ENUM_MAP_ITEM(BindingType::storageBuffer,  vk::DescriptorType::eStorageBuffer)
//    VK_ENUM_MAP_ITEM(BindingType::sampler,         vk::DescriptorType::eCombinedImageSampler)
//    VK_ENUM_MAP_ITEM(BindingType::texture,         vk::DescriptorType::eSampledImage)
//    VK_ENUM_MAP_ITEM(BindingType::storagetTexture, vk::DescriptorType::eStorageImage)
//    VK_ENUM_MAP_END()
//
//    VK_ENUM_MAP_BEGIN(AddressMode, vk::SamplerAddressMode)
//    VK_ENUM_MAP_ITEM(AddressMode::clampToEdge, vk::SamplerAddressMode::eClampToEdge)
//    VK_ENUM_MAP_ITEM(AddressMode::repeat,        vk::SamplerAddressMode::eRepeat)
//    VK_ENUM_MAP_ITEM(AddressMode::mirrorRepeat, vk::SamplerAddressMode::eMirroredRepeat)
//    VK_ENUM_MAP_END()
//
//    VK_ENUM_MAP_BEGIN(FilterMode, vk::Filter)
//    VK_ENUM_MAP_ITEM(FilterMode::nearest, vk::Filter::eNearest)
//    VK_ENUM_MAP_ITEM(FilterMode::linear,  vk::Filter::eLinear)
//    VK_ENUM_MAP_END()
//
//    VK_ENUM_MAP_BEGIN(FilterMode, vk::SamplerMipmapMode)
//    VK_ENUM_MAP_ITEM(FilterMode::nearest, vk::SamplerMipmapMode::eNearest)
//    VK_ENUM_MAP_ITEM(FilterMode::linear,  vk::SamplerMipmapMode::eLinear)
//    VK_ENUM_MAP_END()
//
//    VK_ENUM_MAP_BEGIN(ComparisonFunc, vk::CompareOp)
//    VK_ENUM_MAP_ITEM(ComparisonFunc::never,         vk::CompareOp::eNever)
//    VK_ENUM_MAP_ITEM(ComparisonFunc::less,          vk::CompareOp::eLess)
//    VK_ENUM_MAP_ITEM(ComparisonFunc::equal,         vk::CompareOp::eEqual)
//    VK_ENUM_MAP_ITEM(ComparisonFunc::lessEqual,    vk::CompareOp::eLessOrEqual)
//    VK_ENUM_MAP_ITEM(ComparisonFunc::greater,       vk::CompareOp::eGreater)
//    VK_ENUM_MAP_ITEM(ComparisonFunc::notEqual,     vk::CompareOp::eNotEqual)
//    VK_ENUM_MAP_ITEM(ComparisonFunc::greaterEqual, vk::CompareOp::eGreaterOrEqual)
//    VK_ENUM_MAP_ITEM(ComparisonFunc::always,        vk::CompareOp::eAlways)
//    VK_ENUM_MAP_END()
//
//    VK_ENUM_MAP_BEGIN(StencilOp, vk::StencilOp)
//    VK_ENUM_MAP_ITEM(StencilOp::keep,            vk::StencilOp::eKeep)
//    VK_ENUM_MAP_ITEM(StencilOp::zero,            vk::StencilOp::eZero)
//    VK_ENUM_MAP_ITEM(StencilOp::replace,         vk::StencilOp::eReplace)
//    VK_ENUM_MAP_ITEM(StencilOp::invert,          vk::StencilOp::eInvert)
//    VK_ENUM_MAP_ITEM(StencilOp::incrementClamp, vk::StencilOp::eIncrementAndClamp)
//    VK_ENUM_MAP_ITEM(StencilOp::decrementClamp, vk::StencilOp::eDecrementAndClamp)
//    VK_ENUM_MAP_ITEM(StencilOp::incrementWrap,  vk::StencilOp::eIncrementAndWrap)
//    VK_ENUM_MAP_ITEM(StencilOp::decrementWrap,  vk::StencilOp::eDecrementAndWrap)
//    VK_ENUM_MAP_END()
//
    MTL_ENUM_MAP_BEGIN(LoadOp, MTLLoadAction)
    MTL_ENUM_MAP_ITEM(LoadOp::load,      MTLLoadActionLoad)
    MTL_ENUM_MAP_ITEM(LoadOp::clear,     MTLLoadActionClear)
    MTL_ENUM_MAP_ITEM(LoadOp::max,       MTLLoadActionDontCare)
    MTL_ENUM_MAP_END()

    MTL_ENUM_MAP_BEGIN(StoreOp, MTLStoreAction)
    MTL_ENUM_MAP_ITEM(StoreOp::store,     MTLStoreActionStore)
    MTL_ENUM_MAP_ITEM(StoreOp::discard,   MTLStoreActionDontCare)
    MTL_ENUM_MAP_ITEM(StoreOp::max,       MTLStoreActionUnknown)
    MTL_ENUM_MAP_END()
//
//    VK_ENUM_MAP_BEGIN(IndexFormat, vk::IndexType)
//    VK_ENUM_MAP_ITEM(IndexFormat::uint16, vk::IndexType::eUint16)
//    VK_ENUM_MAP_ITEM(IndexFormat::uint16, vk::IndexType::eUint32)
//    VK_ENUM_MAP_ITEM(IndexFormat::max,    vk::IndexType::eNoneKHR)
//    VK_ENUM_MAP_END()
//
//    VK_ENUM_MAP_BEGIN(TextureState, vk::ImageLayout)
//    VK_ENUM_MAP_ITEM(TextureState::undefined,     vk::ImageLayout::eUndefined)
//    VK_ENUM_MAP_ITEM(TextureState::renderTarget, vk::ImageLayout::eColorAttachmentOptimal)
//    VK_ENUM_MAP_ITEM(TextureState::present,       vk::ImageLayout::ePresentSrcKHR)
//    VK_ENUM_MAP_ITEM(TextureState::max,           vk::ImageLayout::eGeneral)
//    VK_ENUM_MAP_END()

    inline MTLColorWriteMask GetColorWriteMask(ColorWriteFlags mask)
    {
        if (mask == 0) return MTLColorWriteMaskNone;
        if (mask == (ColorWriteBits::red | ColorWriteBits::green | ColorWriteBits::blue | ColorWriteBits::alpha)) return MTLColorWriteMaskAll;
        MTLColorWriteMask res = MTLColorWriteMaskNone;
        if (mask & ColorWriteBits::red) res |= MTLColorWriteMaskRed;
        if (mask & ColorWriteBits::green) res |= MTLColorWriteMaskGreen;
        if (mask & ColorWriteBits::blue) res |= MTLColorWriteMaskBlue;
        if (mask & ColorWriteBits::alpha) res |= MTLColorWriteMaskAlpha;
        return res;
    }

    inline MTLClearColor MakeClearColor(const ColorNormalized<4> &color)
    {
        MTLClearColorMake(color.r, color.g, color.b, color.a);
    }
}
