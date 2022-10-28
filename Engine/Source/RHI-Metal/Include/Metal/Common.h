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
    MTL_ENUM_MAP_ITEM(PixelFormat::R8_UNORM, MTLPixelFormatR8Unorm)
    MTL_ENUM_MAP_ITEM(PixelFormat::R8_SNORM, MTLPixelFormatR8Snorm)
    MTL_ENUM_MAP_ITEM(PixelFormat::R8_UINT,  MTLPixelFormatR8Uint)
    MTL_ENUM_MAP_ITEM(PixelFormat::R8_SINT,  MTLPixelFormatR8Sint)
    // 16-Bits
    MTL_ENUM_MAP_ITEM(PixelFormat::R16_UINT,  MTLPixelFormatR16Uint)
    MTL_ENUM_MAP_ITEM(PixelFormat::R16_SINT,  MTLPixelFormatR16Sint)
    MTL_ENUM_MAP_ITEM(PixelFormat::R16_FLOAT, MTLPixelFormatR16Float)
    MTL_ENUM_MAP_ITEM(PixelFormat::RG8_UNORM, MTLPixelFormatRG8Unorm)
    MTL_ENUM_MAP_ITEM(PixelFormat::RG8_SNORM, MTLPixelFormatRG8Snorm)
    MTL_ENUM_MAP_ITEM(PixelFormat::RG8_UINT,  MTLPixelFormatRG8Uint)
    MTL_ENUM_MAP_ITEM(PixelFormat::RG8_SINT,  MTLPixelFormatRG8Sint)
    // 32-Bits
    MTL_ENUM_MAP_ITEM(PixelFormat::R32_UINT,         MTLPixelFormatR32Uint)
    MTL_ENUM_MAP_ITEM(PixelFormat::R32_SINT,         MTLPixelFormatR32Sint)
    MTL_ENUM_MAP_ITEM(PixelFormat::R32_FLOAT,        MTLPixelFormatR32Float)
    MTL_ENUM_MAP_ITEM(PixelFormat::RG16_UINT,        MTLPixelFormatRG16Uint)
    MTL_ENUM_MAP_ITEM(PixelFormat::RG16_SINT,        MTLPixelFormatRG16Sint)
    MTL_ENUM_MAP_ITEM(PixelFormat::RG16_FLOAT,       MTLPixelFormatRG16Float)
    MTL_ENUM_MAP_ITEM(PixelFormat::RGBA8_UNORM,      MTLPixelFormatRGBA8Unorm)
    MTL_ENUM_MAP_ITEM(PixelFormat::RGBA8_UNORM_SRGB, MTLPixelFormatRGBA8Unorm_sRGB)
    MTL_ENUM_MAP_ITEM(PixelFormat::RGBA8_SNORM,      MTLPixelFormatRGBA8Snorm)
    MTL_ENUM_MAP_ITEM(PixelFormat::RGBA8_UINT,       MTLPixelFormatRGBA8Uint)
    MTL_ENUM_MAP_ITEM(PixelFormat::RGBA8_SINT,       MTLPixelFormatRGBA8Sint)
    MTL_ENUM_MAP_ITEM(PixelFormat::BGRA8_UNORM,      MTLPixelFormatBGRA8Unorm)
    MTL_ENUM_MAP_ITEM(PixelFormat::BGRA8_UNORM_SRGB, MTLPixelFormatBGRA8Unorm_sRGB)
    MTL_ENUM_MAP_ITEM(PixelFormat::RGB9_E5_FLOAT,    MTLPixelFormatRGB9E5Float)
    MTL_ENUM_MAP_ITEM(PixelFormat::RGB10A2_UNORM,    MTLPixelFormatRGB10A2Unorm)
    MTL_ENUM_MAP_ITEM(PixelFormat::RG11B10_FLOAT,    MTLPixelFormatRG11B10Float)
    // 64-Bits
    MTL_ENUM_MAP_ITEM(PixelFormat::RG32_UINT,        MTLPixelFormatRG32Uint)
    MTL_ENUM_MAP_ITEM(PixelFormat::RG32_SINT,        MTLPixelFormatRG32Sint)
    MTL_ENUM_MAP_ITEM(PixelFormat::RG32_FLOAT,       MTLPixelFormatRG32Float)
    MTL_ENUM_MAP_ITEM(PixelFormat::RGBA16_UINT,      MTLPixelFormatRGBA16Uint)
    MTL_ENUM_MAP_ITEM(PixelFormat::RGBA16_SINT,      MTLPixelFormatRGBA16Sint)
    MTL_ENUM_MAP_ITEM(PixelFormat::RGBA16_FLOAT,     MTLPixelFormatRGBA16Float)
    // 128-Bits
    MTL_ENUM_MAP_ITEM(PixelFormat::RGBA32_UINT,      MTLPixelFormatRGBA32Uint)
    MTL_ENUM_MAP_ITEM(PixelFormat::RGBA32_SINT,      MTLPixelFormatRGBA32Sint)
    MTL_ENUM_MAP_ITEM(PixelFormat::RGBA32_FLOAT,     MTLPixelFormatRGBA32Float)
    // Depth-Stencil
    MTL_ENUM_MAP_ITEM(PixelFormat::D16_UNORM,         MTLPixelFormatDepth16Unorm)
    MTL_ENUM_MAP_ITEM(PixelFormat::D24_UNORM_S8_UINT, MTLPixelFormatDepth24Unorm_Stencil8)
    MTL_ENUM_MAP_ITEM(PixelFormat::D32_FLOAT,         MTLPixelFormatDepth32Float)
    // Undefined
    MTL_ENUM_MAP_ITEM(PixelFormat::MAX,               MTLPixelFormatInvalid)
    MTL_ENUM_MAP_END()

//    VK_ENUM_MAP_BEGIN(PrimitiveTopologyType, vk::PrimitiveTopology)
//    VK_ENUM_MAP_ITEM(PrimitiveTopologyType::POINT, vk::PrimitiveTopology::ePointList)
//    VK_ENUM_MAP_ITEM(PrimitiveTopologyType::LINE, vk::PrimitiveTopology::eLineList)
//    VK_ENUM_MAP_ITEM(PrimitiveTopologyType::TRIANGLE, vk::PrimitiveTopology::eTriangleList)
//    VK_ENUM_MAP_END()
//
//    VK_ENUM_MAP_BEGIN(CullMode, vk::CullModeFlagBits)
//    VK_ENUM_MAP_ITEM(CullMode::NONE, vk::CullModeFlagBits::eNone)
//    VK_ENUM_MAP_ITEM(CullMode::FRONT, vk::CullModeFlagBits::eFront)
//    VK_ENUM_MAP_ITEM(CullMode::BACK, vk::CullModeFlagBits::eBack)
//    VK_ENUM_MAP_END()
//
//    VK_ENUM_MAP_BEGIN(BlendOp, vk::BlendOp)
//    VK_ENUM_MAP_ITEM(BlendOp::OP_ADD,              vk::BlendOp::eAdd)
//    VK_ENUM_MAP_ITEM(BlendOp::OP_SUBTRACT,         vk::BlendOp::eSubtract)
//    VK_ENUM_MAP_ITEM(BlendOp::OP_REVERSE_SUBTRACT, vk::BlendOp::eReverseSubtract)
//    VK_ENUM_MAP_ITEM(BlendOp::OP_MIN,              vk::BlendOp::eMin)
//    VK_ENUM_MAP_ITEM(BlendOp::OP_MAX,              vk::BlendOp::eMax)
//    VK_ENUM_MAP_END()
//
//    VK_ENUM_MAP_BEGIN(BlendFactor, vk::BlendFactor)
//    VK_ENUM_MAP_ITEM(BlendFactor::ZERO,                vk::BlendFactor::eZero)
//    VK_ENUM_MAP_ITEM(BlendFactor::ONE,                 vk::BlendFactor::eOne)
//    VK_ENUM_MAP_ITEM(BlendFactor::SRC,                 vk::BlendFactor::eSrcColor)
//    VK_ENUM_MAP_ITEM(BlendFactor::ONE_MINUS_SRC,       vk::BlendFactor::eOneMinusSrcColor)
//    VK_ENUM_MAP_ITEM(BlendFactor::SRC_ALPHA,           vk::BlendFactor::eSrcAlpha)
//    VK_ENUM_MAP_ITEM(BlendFactor::ONE_MINUS_SRC_ALPHA, vk::BlendFactor::eOneMinusSrcAlpha)
//    VK_ENUM_MAP_ITEM(BlendFactor::DST,                 vk::BlendFactor::eDstColor)
//    VK_ENUM_MAP_ITEM(BlendFactor::ONE_MINUS_DST,       vk::BlendFactor::eOneMinusDstColor)
//    VK_ENUM_MAP_ITEM(BlendFactor::DST_ALPHA,           vk::BlendFactor::eDstAlpha)
//    VK_ENUM_MAP_ITEM(BlendFactor::ONE_MINUS_DST_ALPHA, vk::BlendFactor::eOneMinusDstAlpha)
//    VK_ENUM_MAP_END()
//
    MTL_ENUM_MAP_BEGIN(VertexFormat, MTLVertexFormat)
    MTL_ENUM_MAP_ITEM(VertexFormat::UINT8_X2,   MTLVertexFormatUChar2)
    MTL_ENUM_MAP_ITEM(VertexFormat::UINT8_X4,   MTLVertexFormatUChar4)
    MTL_ENUM_MAP_ITEM(VertexFormat::SINT8_X2,   MTLVertexFormatChar2)
    MTL_ENUM_MAP_ITEM(VertexFormat::SINT8_X4,   MTLVertexFormatChar4)
    MTL_ENUM_MAP_ITEM(VertexFormat::UNORM8_X2,  MTLVertexFormatUChar2Normalized)
    MTL_ENUM_MAP_ITEM(VertexFormat::UNORM8_X4,  MTLVertexFormatUChar4Normalized)
    MTL_ENUM_MAP_ITEM(VertexFormat::SNORM8_X2,  MTLVertexFormatChar2Normalized)
    MTL_ENUM_MAP_ITEM(VertexFormat::SNORM8_X4,  MTLVertexFormatChar4Normalized)
    MTL_ENUM_MAP_ITEM(VertexFormat::UINT16_X2,  MTLVertexFormatUShort2)
    MTL_ENUM_MAP_ITEM(VertexFormat::UINT16_X4,  MTLVertexFormatUShort4)
    MTL_ENUM_MAP_ITEM(VertexFormat::SINT16_X2,  MTLVertexFormatShort2)
    MTL_ENUM_MAP_ITEM(VertexFormat::SINT16_X4,  MTLVertexFormatShort4)
    MTL_ENUM_MAP_ITEM(VertexFormat::UNORM16_X2, MTLVertexFormatUShort2Normalized)
    MTL_ENUM_MAP_ITEM(VertexFormat::UNORM16_X4, MTLVertexFormatUShort4Normalized)
    MTL_ENUM_MAP_ITEM(VertexFormat::SNORM16_X2, MTLVertexFormatShort2Normalized)
    MTL_ENUM_MAP_ITEM(VertexFormat::SNORM16_X4, MTLVertexFormatShort4Normalized)
    MTL_ENUM_MAP_ITEM(VertexFormat::FLOAT16_X2, MTLVertexFormatHalf2)
    MTL_ENUM_MAP_ITEM(VertexFormat::FLOAT16_X4, MTLVertexFormatHalf4)
    MTL_ENUM_MAP_ITEM(VertexFormat::FLOAT32_X1, MTLVertexFormatFloat)
    MTL_ENUM_MAP_ITEM(VertexFormat::FLOAT32_X2, MTLVertexFormatFloat2)
    MTL_ENUM_MAP_ITEM(VertexFormat::FLOAT32_X3, MTLVertexFormatFloat3)
    MTL_ENUM_MAP_ITEM(VertexFormat::FLOAT32_X4, MTLVertexFormatFloat4)
    MTL_ENUM_MAP_ITEM(VertexFormat::UINT32_X1,  MTLVertexFormatUInt)
    MTL_ENUM_MAP_ITEM(VertexFormat::UINT32_X2,  MTLVertexFormatUInt2)
    MTL_ENUM_MAP_ITEM(VertexFormat::UINT32_X3,  MTLVertexFormatUInt3)
    MTL_ENUM_MAP_ITEM(VertexFormat::UINT32_X4,  MTLVertexFormatUInt4)
    MTL_ENUM_MAP_ITEM(VertexFormat::SINT32_X1,  MTLVertexFormatInt)
    MTL_ENUM_MAP_ITEM(VertexFormat::SINT32_X2,  MTLVertexFormatInt2)
    MTL_ENUM_MAP_ITEM(VertexFormat::SINT32_X3,  MTLVertexFormatInt3)
    MTL_ENUM_MAP_ITEM(VertexFormat::SINT32_X4,  MTLVertexFormatInt4)
    MTL_ENUM_MAP_END()
//
//    VK_ENUM_MAP_BEGIN(BindingType, vk::DescriptorType)
//    VK_ENUM_MAP_ITEM(BindingType::UNIFORM_BUFFER,  vk::DescriptorType::eUniformBuffer)
//    VK_ENUM_MAP_ITEM(BindingType::STORAGE_BUFFER,  vk::DescriptorType::eStorageBuffer)
//    VK_ENUM_MAP_ITEM(BindingType::SAMPLER,         vk::DescriptorType::eCombinedImageSampler)
//    VK_ENUM_MAP_ITEM(BindingType::TEXTURE,         vk::DescriptorType::eSampledImage)
//    VK_ENUM_MAP_ITEM(BindingType::STORAGE_TEXTURE, vk::DescriptorType::eStorageImage)
//    VK_ENUM_MAP_END()
//
//    VK_ENUM_MAP_BEGIN(AddressMode, vk::SamplerAddressMode)
//    VK_ENUM_MAP_ITEM(AddressMode::CLAMP_TO_EDGE, vk::SamplerAddressMode::eClampToEdge)
//    VK_ENUM_MAP_ITEM(AddressMode::REPEAT,        vk::SamplerAddressMode::eRepeat)
//    VK_ENUM_MAP_ITEM(AddressMode::MIRROR_REPEAT, vk::SamplerAddressMode::eMirroredRepeat)
//    VK_ENUM_MAP_END()
//
//    VK_ENUM_MAP_BEGIN(FilterMode, vk::Filter)
//    VK_ENUM_MAP_ITEM(FilterMode::NEAREST, vk::Filter::eNearest)
//    VK_ENUM_MAP_ITEM(FilterMode::LINEAR,  vk::Filter::eLinear)
//    VK_ENUM_MAP_END()
//
//    VK_ENUM_MAP_BEGIN(FilterMode, vk::SamplerMipmapMode)
//    VK_ENUM_MAP_ITEM(FilterMode::NEAREST, vk::SamplerMipmapMode::eNearest)
//    VK_ENUM_MAP_ITEM(FilterMode::LINEAR,  vk::SamplerMipmapMode::eLinear)
//    VK_ENUM_MAP_END()
//
//    VK_ENUM_MAP_BEGIN(ComparisonFunc, vk::CompareOp)
//    VK_ENUM_MAP_ITEM(ComparisonFunc::NEVER,         vk::CompareOp::eNever)
//    VK_ENUM_MAP_ITEM(ComparisonFunc::LESS,          vk::CompareOp::eLess)
//    VK_ENUM_MAP_ITEM(ComparisonFunc::EQUAL,         vk::CompareOp::eEqual)
//    VK_ENUM_MAP_ITEM(ComparisonFunc::LESS_EQUAL,    vk::CompareOp::eLessOrEqual)
//    VK_ENUM_MAP_ITEM(ComparisonFunc::GREATER,       vk::CompareOp::eGreater)
//    VK_ENUM_MAP_ITEM(ComparisonFunc::NOT_EQUAL,     vk::CompareOp::eNotEqual)
//    VK_ENUM_MAP_ITEM(ComparisonFunc::GREATER_EQUAL, vk::CompareOp::eGreaterOrEqual)
//    VK_ENUM_MAP_ITEM(ComparisonFunc::ALWAYS,        vk::CompareOp::eAlways)
//    VK_ENUM_MAP_END()
//
//    VK_ENUM_MAP_BEGIN(StencilOp, vk::StencilOp)
//    VK_ENUM_MAP_ITEM(StencilOp::KEEP,            vk::StencilOp::eKeep)
//    VK_ENUM_MAP_ITEM(StencilOp::ZERO,            vk::StencilOp::eZero)
//    VK_ENUM_MAP_ITEM(StencilOp::REPLACE,         vk::StencilOp::eReplace)
//    VK_ENUM_MAP_ITEM(StencilOp::INVERT,          vk::StencilOp::eInvert)
//    VK_ENUM_MAP_ITEM(StencilOp::INCREMENT_CLAMP, vk::StencilOp::eIncrementAndClamp)
//    VK_ENUM_MAP_ITEM(StencilOp::DECREMENT_CLAMP, vk::StencilOp::eDecrementAndClamp)
//    VK_ENUM_MAP_ITEM(StencilOp::INCREMENT_WRAP,  vk::StencilOp::eIncrementAndWrap)
//    VK_ENUM_MAP_ITEM(StencilOp::DECREMENT_WRAP,  vk::StencilOp::eDecrementAndWrap)
//    VK_ENUM_MAP_END()
//
//    VK_ENUM_MAP_BEGIN(LoadOp, vk::AttachmentLoadOp)
//    VK_ENUM_MAP_ITEM(LoadOp::LOAD,  vk::AttachmentLoadOp::eLoad)
//    VK_ENUM_MAP_ITEM(LoadOp::CLEAR, vk::AttachmentLoadOp::eClear)
//    VK_ENUM_MAP_ITEM(LoadOp::MAX,   vk::AttachmentLoadOp::eNoneEXT)
//    VK_ENUM_MAP_END()
//
//    VK_ENUM_MAP_BEGIN(StoreOp, vk::AttachmentStoreOp)
//    VK_ENUM_MAP_ITEM(StoreOp::STORE,   vk::AttachmentStoreOp::eStore)
//    VK_ENUM_MAP_ITEM(StoreOp::DISCARD, vk::AttachmentStoreOp::eDontCare)
//    VK_ENUM_MAP_ITEM(StoreOp::MAX,     vk::AttachmentStoreOp::eNoneEXT)
//    VK_ENUM_MAP_END()
//
//    VK_ENUM_MAP_BEGIN(IndexFormat, vk::IndexType)
//    VK_ENUM_MAP_ITEM(IndexFormat::UINT16, vk::IndexType::eUint16)
//    VK_ENUM_MAP_ITEM(IndexFormat::UINT16, vk::IndexType::eUint32)
//    VK_ENUM_MAP_ITEM(IndexFormat::MAX,    vk::IndexType::eNoneKHR)
//    VK_ENUM_MAP_END()
//
//    VK_ENUM_MAP_BEGIN(TextureState, vk::ImageLayout)
//    VK_ENUM_MAP_ITEM(TextureState::UNDEFINED,     vk::ImageLayout::eUndefined)
//    VK_ENUM_MAP_ITEM(TextureState::RENDER_TARGET, vk::ImageLayout::eColorAttachmentOptimal)
//    VK_ENUM_MAP_ITEM(TextureState::PRESENT,       vk::ImageLayout::ePresentSrcKHR)
//    VK_ENUM_MAP_ITEM(TextureState::MAX,           vk::ImageLayout::eGeneral)
//    VK_ENUM_MAP_END()

    inline MTLColorWriteMask GetColorWriteMask(ColorWriteFlags mask)
    {
        if (mask == 0) return MTLColorWriteMaskNone;
        if (mask == (ColorWriteBits::RED | ColorWriteBits::GREEN | ColorWriteBits::BLUE | ColorWriteBits::ALPHA)) return MTLColorWriteMaskAll;
        MTLColorWriteMask res = MTLColorWriteMaskNone;
        if (mask & ColorWriteBits::RED) res |= MTLColorWriteMaskRed;
        if (mask & ColorWriteBits::GREEN) res |= MTLColorWriteMaskGreen;
        if (mask & ColorWriteBits::BLUE) res |= MTLColorWriteMaskBlue;
        if (mask & ColorWriteBits::ALPHA) res |= MTLColorWriteMaskAlpha;
        return res;
    }
}
