//
// Created by johnk on 11/1/2022.
//

#pragma once

#include <Common/Debug.h>
#include <RHI/Common.h>
#include <unordered_map>
#include <vulkan/vulkan.hpp>

namespace RHI::Vulkan {
#define VK_KHRONOS_VALIDATION_LAYER_NAME "VK_LAYER_KHRONOS_validation"
}

// duplicated code because static variable and namespace
namespace RHI::Vulkan {
    template <typename A, typename B>
    static const std::unordered_map<A, B> VK_ENUM_MAP;

    template <typename A, typename B>
    B VKEnumCast(const A& value)
    {
        auto iter = VK_ENUM_MAP<A, B>.find(value);
        Assert((iter != VK_ENUM_MAP<A, B>.end()));
        return static_cast<B>(iter->second);
    }

#define VK_ENUM_MAP_BEGIN(A, B) template <> static const std::unordered_map<A, B> VK_ENUM_MAP<A, B> = {
#define VK_ENUM_MAP_ITEM(A, B) { A, B },
#define VK_ENUM_MAP_END() };
}

// enum map definitions
namespace RHI::Vulkan {
    VK_ENUM_MAP_BEGIN(vk::PhysicalDeviceType, GpuType)
        VK_ENUM_MAP_ITEM(vk::PhysicalDeviceType::eOther,         GpuType::software)
        VK_ENUM_MAP_ITEM(vk::PhysicalDeviceType::eIntegratedGpu, GpuType::hardware)
        VK_ENUM_MAP_ITEM(vk::PhysicalDeviceType::eDiscreteGpu,   GpuType::hardware)
        VK_ENUM_MAP_ITEM(vk::PhysicalDeviceType::eVirtualGpu,    GpuType::software)
        VK_ENUM_MAP_ITEM(vk::PhysicalDeviceType::eCpu,           GpuType::software)
    VK_ENUM_MAP_END()

    VK_ENUM_MAP_BEGIN(PixelFormat, vk::Format)
        // 8-Bits
        VK_ENUM_MAP_ITEM(PixelFormat::r8Unorm, vk::Format::eR8Unorm)
        VK_ENUM_MAP_ITEM(PixelFormat::r8Snorm, vk::Format::eR8Snorm)
        VK_ENUM_MAP_ITEM(PixelFormat::r8Uint,  vk::Format::eR8Uint)
        VK_ENUM_MAP_ITEM(PixelFormat::r8Sint,  vk::Format::eR8Sint)
        // 16-Bits
        VK_ENUM_MAP_ITEM(PixelFormat::r16Uint,  vk::Format::eR16Uint)
        VK_ENUM_MAP_ITEM(PixelFormat::r16Sint,  vk::Format::eR16Sint)
        VK_ENUM_MAP_ITEM(PixelFormat::r16Float, vk::Format::eR16Sfloat)
        VK_ENUM_MAP_ITEM(PixelFormat::rg8Unorm, vk::Format::eR8G8Unorm)
        VK_ENUM_MAP_ITEM(PixelFormat::rg8Snorm, vk::Format::eR8G8Snorm)
        VK_ENUM_MAP_ITEM(PixelFormat::rg8Uint,  vk::Format::eR8G8Uint)
        VK_ENUM_MAP_ITEM(PixelFormat::rg8Sint,  vk::Format::eR8G8Sint)
        // 32-Bits
        VK_ENUM_MAP_ITEM(PixelFormat::r32Uint,         vk::Format::eR32Uint)
        VK_ENUM_MAP_ITEM(PixelFormat::r32Sint,         vk::Format::eR32Sint)
        VK_ENUM_MAP_ITEM(PixelFormat::r32Float,        vk::Format::eR32Sfloat)
        VK_ENUM_MAP_ITEM(PixelFormat::rg16Uint,        vk::Format::eR16G16Uint)
        VK_ENUM_MAP_ITEM(PixelFormat::rg16Sint,        vk::Format::eR16G16Sint)
        VK_ENUM_MAP_ITEM(PixelFormat::rg16Float,       vk::Format::eR16G16Sfloat)
        VK_ENUM_MAP_ITEM(PixelFormat::rgba8Unorm,      vk::Format::eR8G8B8A8Unorm)
        VK_ENUM_MAP_ITEM(PixelFormat::rgba8UnormSrgb, vk::Format::eR8G8B8A8Srgb)
        VK_ENUM_MAP_ITEM(PixelFormat::rgba8Snorm,      vk::Format::eR8G8B8A8Snorm)
        VK_ENUM_MAP_ITEM(PixelFormat::rgba8Uint,       vk::Format::eR8G8B8A8Uint)
        VK_ENUM_MAP_ITEM(PixelFormat::rgba8Sint,       vk::Format::eR8G8B8A8Sint)
        VK_ENUM_MAP_ITEM(PixelFormat::bgra8Unorm,      vk::Format::eB8G8R8A8Unorm)
        VK_ENUM_MAP_ITEM(PixelFormat::bgra8UnormSrgb, vk::Format::eB8G8R8A8Srgb)
        VK_ENUM_MAP_ITEM(PixelFormat::rgb9E5Float,    vk::Format::eE5B9G9R9UfloatPack32)
        VK_ENUM_MAP_ITEM(PixelFormat::rgb10A2Unorm,    vk::Format::eA2R10G10B10UnormPack32)
        VK_ENUM_MAP_ITEM(PixelFormat::rg11B10Float,    vk::Format::eB10G11R11UfloatPack32)
        // 64-Bits
        VK_ENUM_MAP_ITEM(PixelFormat::rg32Uint,        vk::Format::eR32G32Uint)
        VK_ENUM_MAP_ITEM(PixelFormat::rg32Sint,        vk::Format::eR32G32Sint)
        VK_ENUM_MAP_ITEM(PixelFormat::rg32Float,       vk::Format::eR32G32Sfloat)
        VK_ENUM_MAP_ITEM(PixelFormat::rgba16Uint,      vk::Format::eR16G16B16A16Uint)
        VK_ENUM_MAP_ITEM(PixelFormat::rgba16Sint,      vk::Format::eR16G16B16A16Sint)
        VK_ENUM_MAP_ITEM(PixelFormat::rgba16Float,     vk::Format::eR16G16B16A16Sfloat)
        // 128-Bits
        VK_ENUM_MAP_ITEM(PixelFormat::rgba32Uint,      vk::Format::eR32G32B32A32Uint)
        VK_ENUM_MAP_ITEM(PixelFormat::rgba32Sint,      vk::Format::eR32G32B32A32Sint)
        VK_ENUM_MAP_ITEM(PixelFormat::rgba32Float,     vk::Format::eR32G32B32A32Sfloat)
        // Depth-Stencil
        VK_ENUM_MAP_ITEM(PixelFormat::d16Unorm,         vk::Format::eD16Unorm)
        VK_ENUM_MAP_ITEM(PixelFormat::d24UnormS8Uint, vk::Format::eD24UnormS8Uint)
        VK_ENUM_MAP_ITEM(PixelFormat::d32Float,         vk::Format::eD32Sfloat)
        VK_ENUM_MAP_ITEM(PixelFormat::d32FloatS8Uint, vk::Format::eD32SfloatS8Uint)
        // Undefined
        VK_ENUM_MAP_ITEM(PixelFormat::max,               vk::Format::eUndefined)
    VK_ENUM_MAP_END()

    VK_ENUM_MAP_BEGIN(QueueType, vk::QueueFlagBits)
        VK_ENUM_MAP_ITEM(QueueType::graphics, vk::QueueFlagBits::eGraphics)
        VK_ENUM_MAP_ITEM(QueueType::compute,  vk::QueueFlagBits::eCompute)
        VK_ENUM_MAP_ITEM(QueueType::transfer, vk::QueueFlagBits::eTransfer)
    VK_ENUM_MAP_END()

    VK_ENUM_MAP_BEGIN(TextureDimension, vk::ImageType)
        VK_ENUM_MAP_ITEM(TextureDimension::t1D, vk::ImageType::e1D)
        VK_ENUM_MAP_ITEM(TextureDimension::t2D, vk::ImageType::e2D)
        VK_ENUM_MAP_ITEM(TextureDimension::t3D, vk::ImageType::e3D)
    VK_ENUM_MAP_END()

    VK_ENUM_MAP_BEGIN(TextureViewDimension, vk::ImageViewType)
        VK_ENUM_MAP_ITEM(TextureViewDimension::tv1D,         vk::ImageViewType::e1D)
        VK_ENUM_MAP_ITEM(TextureViewDimension::tv2D,         vk::ImageViewType::e2D)
        VK_ENUM_MAP_ITEM(TextureViewDimension::tv2DArray,   vk::ImageViewType::e2DArray)
        VK_ENUM_MAP_ITEM(TextureViewDimension::tvCube,       vk::ImageViewType::eCube)
        VK_ENUM_MAP_ITEM(TextureViewDimension::tvCubeArray, vk::ImageViewType::eCubeArray)
        VK_ENUM_MAP_ITEM(TextureViewDimension::tv3D,         vk::ImageViewType::e3D)
    VK_ENUM_MAP_END()

    VK_ENUM_MAP_BEGIN(ShaderStageBits, vk::ShaderStageFlagBits)
        VK_ENUM_MAP_ITEM(ShaderStageBits::sVertex,   vk::ShaderStageFlagBits::eVertex)
        VK_ENUM_MAP_ITEM(ShaderStageBits::sPixel, vk::ShaderStageFlagBits::eFragment)
        VK_ENUM_MAP_ITEM(ShaderStageBits::sCompute,  vk::ShaderStageFlagBits::eCompute)
    VK_ENUM_MAP_END()

    VK_ENUM_MAP_BEGIN(PrimitiveTopologyType, vk::PrimitiveTopology)
        VK_ENUM_MAP_ITEM(PrimitiveTopologyType::point, vk::PrimitiveTopology::ePointList)
        VK_ENUM_MAP_ITEM(PrimitiveTopologyType::line, vk::PrimitiveTopology::eLineList)
        VK_ENUM_MAP_ITEM(PrimitiveTopologyType::triangle, vk::PrimitiveTopology::eTriangleList)
    VK_ENUM_MAP_END()

    VK_ENUM_MAP_BEGIN(CullMode, vk::CullModeFlagBits)
        VK_ENUM_MAP_ITEM(CullMode::none, vk::CullModeFlagBits::eNone)
        VK_ENUM_MAP_ITEM(CullMode::front, vk::CullModeFlagBits::eFront)
        VK_ENUM_MAP_ITEM(CullMode::back, vk::CullModeFlagBits::eBack)
    VK_ENUM_MAP_END()

    VK_ENUM_MAP_BEGIN(BlendOp, vk::BlendOp)
        VK_ENUM_MAP_ITEM(BlendOp::opAdd,              vk::BlendOp::eAdd)
        VK_ENUM_MAP_ITEM(BlendOp::opSubstract,         vk::BlendOp::eSubtract)
        VK_ENUM_MAP_ITEM(BlendOp::opReverseSubstract, vk::BlendOp::eReverseSubtract)
        VK_ENUM_MAP_ITEM(BlendOp::opMin,              vk::BlendOp::eMin)
        VK_ENUM_MAP_ITEM(BlendOp::opMax,              vk::BlendOp::eMax)
    VK_ENUM_MAP_END()

    VK_ENUM_MAP_BEGIN(BlendFactor, vk::BlendFactor)
        VK_ENUM_MAP_ITEM(BlendFactor::zero,                vk::BlendFactor::eZero)
        VK_ENUM_MAP_ITEM(BlendFactor::one,                 vk::BlendFactor::eOne)
        VK_ENUM_MAP_ITEM(BlendFactor::src,                 vk::BlendFactor::eSrcColor)
        VK_ENUM_MAP_ITEM(BlendFactor::oneMinusSrc,       vk::BlendFactor::eOneMinusSrcColor)
        VK_ENUM_MAP_ITEM(BlendFactor::srcAlpha,           vk::BlendFactor::eSrcAlpha)
        VK_ENUM_MAP_ITEM(BlendFactor::oneMinusSrcAlpha, vk::BlendFactor::eOneMinusSrcAlpha)
        VK_ENUM_MAP_ITEM(BlendFactor::dst,                 vk::BlendFactor::eDstColor)
        VK_ENUM_MAP_ITEM(BlendFactor::oneMinusDst,       vk::BlendFactor::eOneMinusDstColor)
        VK_ENUM_MAP_ITEM(BlendFactor::dstAlpha,           vk::BlendFactor::eDstAlpha)
        VK_ENUM_MAP_ITEM(BlendFactor::oneMinusDstAlpha, vk::BlendFactor::eOneMinusDstAlpha)
    VK_ENUM_MAP_END()

    VK_ENUM_MAP_BEGIN(VertexFormat, vk::Format)
        VK_ENUM_MAP_ITEM(VertexFormat::uint8X2,   vk::Format::eR8G8Uint)
        VK_ENUM_MAP_ITEM(VertexFormat::uint8X4,   vk::Format::eR8G8B8A8Uint)
        VK_ENUM_MAP_ITEM(VertexFormat::sint8X2,   vk::Format::eR8G8Sint)
        VK_ENUM_MAP_ITEM(VertexFormat::sint8X4,   vk::Format::eR8G8B8A8Sint)
        VK_ENUM_MAP_ITEM(VertexFormat::unorm8X2,  vk::Format::eR8G8Unorm)
        VK_ENUM_MAP_ITEM(VertexFormat::unorm8X4,  vk::Format::eR8G8B8A8Unorm)
        VK_ENUM_MAP_ITEM(VertexFormat::snorm8X2,  vk::Format::eR8G8Snorm)
        VK_ENUM_MAP_ITEM(VertexFormat::snorm8X4,  vk::Format::eR8G8B8A8Unorm)
        VK_ENUM_MAP_ITEM(VertexFormat::uint16X2,  vk::Format::eR16G16Uint)
        VK_ENUM_MAP_ITEM(VertexFormat::uint16X4,  vk::Format::eR16G16B16A16Uint)
        VK_ENUM_MAP_ITEM(VertexFormat::sint16X2,  vk::Format::eR16G16Sint)
        VK_ENUM_MAP_ITEM(VertexFormat::sint16X4,  vk::Format::eR16G16B16A16Sint)
        VK_ENUM_MAP_ITEM(VertexFormat::unorm16X2, vk::Format::eR16G16Unorm)
        VK_ENUM_MAP_ITEM(VertexFormat::unorm16X4, vk::Format::eR16G16B16A16Unorm)
        VK_ENUM_MAP_ITEM(VertexFormat::snorm16X2, vk::Format::eR16G16Snorm)
        VK_ENUM_MAP_ITEM(VertexFormat::snorm16X4, vk::Format::eR16G16B16A16Snorm)
        VK_ENUM_MAP_ITEM(VertexFormat::float16X2, vk::Format::eR16G16Sfloat)
        VK_ENUM_MAP_ITEM(VertexFormat::float16X4, vk::Format::eR16G16B16A16Sfloat)
        VK_ENUM_MAP_ITEM(VertexFormat::float32X1, vk::Format::eR32Sfloat)
        VK_ENUM_MAP_ITEM(VertexFormat::float32X2, vk::Format::eR32G32Sfloat)
        VK_ENUM_MAP_ITEM(VertexFormat::float32X3, vk::Format::eR32G32B32Sfloat)
        VK_ENUM_MAP_ITEM(VertexFormat::float32X4, vk::Format::eR32G32B32A32Sfloat)
        VK_ENUM_MAP_ITEM(VertexFormat::uint32X1,  vk::Format::eR32Uint)
        VK_ENUM_MAP_ITEM(VertexFormat::uint32X2,  vk::Format::eR32G32Uint)
        VK_ENUM_MAP_ITEM(VertexFormat::uint32X3,  vk::Format::eR32G32B32Uint)
        VK_ENUM_MAP_ITEM(VertexFormat::uint32X4,  vk::Format::eR32G32B32A32Uint)
        VK_ENUM_MAP_ITEM(VertexFormat::sint32X1,  vk::Format::eR32Sint)
        VK_ENUM_MAP_ITEM(VertexFormat::sint32X2,  vk::Format::eR32G32Sint)
        VK_ENUM_MAP_ITEM(VertexFormat::sint32X3,  vk::Format::eR32G32B32Sint)
        VK_ENUM_MAP_ITEM(VertexFormat::sint32X4,  vk::Format::eR32G32B32A32Sint)
    VK_ENUM_MAP_END()

    VK_ENUM_MAP_BEGIN(BindingType, vk::DescriptorType)
        VK_ENUM_MAP_ITEM(BindingType::uniformBuffer,  vk::DescriptorType::eUniformBuffer)
        VK_ENUM_MAP_ITEM(BindingType::storageBuffer,  vk::DescriptorType::eStorageBuffer)
        VK_ENUM_MAP_ITEM(BindingType::sampler,         vk::DescriptorType::eSampler)
        VK_ENUM_MAP_ITEM(BindingType::texture,         vk::DescriptorType::eSampledImage)
        VK_ENUM_MAP_ITEM(BindingType::storageTexture, vk::DescriptorType::eStorageImage)
    VK_ENUM_MAP_END()

    VK_ENUM_MAP_BEGIN(AddressMode, vk::SamplerAddressMode)
        VK_ENUM_MAP_ITEM(AddressMode::clampToEdge, vk::SamplerAddressMode::eClampToEdge)
        VK_ENUM_MAP_ITEM(AddressMode::repeat,        vk::SamplerAddressMode::eRepeat)
        VK_ENUM_MAP_ITEM(AddressMode::mirrorRepeat, vk::SamplerAddressMode::eMirroredRepeat)
    VK_ENUM_MAP_END()

    VK_ENUM_MAP_BEGIN(FilterMode, vk::Filter)
        VK_ENUM_MAP_ITEM(FilterMode::nearest, vk::Filter::eNearest)
        VK_ENUM_MAP_ITEM(FilterMode::linear,  vk::Filter::eLinear)
    VK_ENUM_MAP_END()

    VK_ENUM_MAP_BEGIN(FilterMode, vk::SamplerMipmapMode)
        VK_ENUM_MAP_ITEM(FilterMode::nearest, vk::SamplerMipmapMode::eNearest)
        VK_ENUM_MAP_ITEM(FilterMode::linear,  vk::SamplerMipmapMode::eLinear)
    VK_ENUM_MAP_END()

    VK_ENUM_MAP_BEGIN(ComparisonFunc, vk::CompareOp)
        VK_ENUM_MAP_ITEM(ComparisonFunc::never,         vk::CompareOp::eNever)
        VK_ENUM_MAP_ITEM(ComparisonFunc::less,          vk::CompareOp::eLess)
        VK_ENUM_MAP_ITEM(ComparisonFunc::equal,         vk::CompareOp::eEqual)
        VK_ENUM_MAP_ITEM(ComparisonFunc::lessEqual,    vk::CompareOp::eLessOrEqual)
        VK_ENUM_MAP_ITEM(ComparisonFunc::greater,       vk::CompareOp::eGreater)
        VK_ENUM_MAP_ITEM(ComparisonFunc::notEqual,     vk::CompareOp::eNotEqual)
        VK_ENUM_MAP_ITEM(ComparisonFunc::greaterEqual, vk::CompareOp::eGreaterOrEqual)
        VK_ENUM_MAP_ITEM(ComparisonFunc::always,        vk::CompareOp::eAlways)
    VK_ENUM_MAP_END()

    VK_ENUM_MAP_BEGIN(StencilOp, vk::StencilOp)
        VK_ENUM_MAP_ITEM(StencilOp::keep,            vk::StencilOp::eKeep)
        VK_ENUM_MAP_ITEM(StencilOp::zero,            vk::StencilOp::eZero)
        VK_ENUM_MAP_ITEM(StencilOp::replace,         vk::StencilOp::eReplace)
        VK_ENUM_MAP_ITEM(StencilOp::invert,          vk::StencilOp::eInvert)
        VK_ENUM_MAP_ITEM(StencilOp::incrementClamp, vk::StencilOp::eIncrementAndClamp)
        VK_ENUM_MAP_ITEM(StencilOp::decrementClamp, vk::StencilOp::eDecrementAndClamp)
        VK_ENUM_MAP_ITEM(StencilOp::incrementWrap,  vk::StencilOp::eIncrementAndWrap)
        VK_ENUM_MAP_ITEM(StencilOp::decrementWrap,  vk::StencilOp::eDecrementAndWrap)
    VK_ENUM_MAP_END()

    VK_ENUM_MAP_BEGIN(LoadOp, vk::AttachmentLoadOp)
        VK_ENUM_MAP_ITEM(LoadOp::load,  vk::AttachmentLoadOp::eLoad)
        VK_ENUM_MAP_ITEM(LoadOp::clear, vk::AttachmentLoadOp::eClear)
        VK_ENUM_MAP_ITEM(LoadOp::max,   vk::AttachmentLoadOp::eNoneEXT)
    VK_ENUM_MAP_END()

    VK_ENUM_MAP_BEGIN(StoreOp, vk::AttachmentStoreOp)
        VK_ENUM_MAP_ITEM(StoreOp::store,   vk::AttachmentStoreOp::eStore)
        VK_ENUM_MAP_ITEM(StoreOp::discard, vk::AttachmentStoreOp::eDontCare)
        VK_ENUM_MAP_ITEM(StoreOp::max,     vk::AttachmentStoreOp::eNoneEXT)
    VK_ENUM_MAP_END()

    VK_ENUM_MAP_BEGIN(IndexFormat, vk::IndexType)
        VK_ENUM_MAP_ITEM(IndexFormat::uint16, vk::IndexType::eUint16)
        VK_ENUM_MAP_ITEM(IndexFormat::uint32, vk::IndexType::eUint32)
        VK_ENUM_MAP_ITEM(IndexFormat::max,    vk::IndexType::eNoneKHR)
    VK_ENUM_MAP_END()

    VK_ENUM_MAP_BEGIN(TextureState, vk::ImageLayout)
        VK_ENUM_MAP_ITEM(TextureState::undefined,     vk::ImageLayout::eUndefined)
        VK_ENUM_MAP_ITEM(TextureState::renderTarget, vk::ImageLayout::eColorAttachmentOptimal)
        VK_ENUM_MAP_ITEM(TextureState::present,       vk::ImageLayout::ePresentSrcKHR)
        VK_ENUM_MAP_ITEM(TextureState::max,           vk::ImageLayout::eGeneral)
    VK_ENUM_MAP_END()

    VK_ENUM_MAP_BEGIN(PresentMode, vk::PresentModeKHR)
        VK_ENUM_MAP_ITEM(PresentMode::immediately, vk::PresentModeKHR::eImmediate)
        VK_ENUM_MAP_ITEM(PresentMode::vsync,       vk::PresentModeKHR::eFifo)
        VK_ENUM_MAP_ITEM(PresentMode::max,         vk::PresentModeKHR::eImmediate) // TODO Set the default present mode to immediate?
    VK_ENUM_MAP_END()

    inline vk::Extent3D FromRHI(const RHI::Extent<3>& ext)
    {
        return { static_cast<uint32_t>(ext.x), static_cast<uint32_t>(ext.y), static_cast<uint32_t>(ext.z) };
    }

    inline vk::ShaderStageFlags FromRHI(const ShaderStageFlags& src)
    {
        vk::ShaderStageFlags flags = {};
        for (auto& pair : VK_ENUM_MAP<ShaderStageBits, vk::ShaderStageFlagBits>) {
            if (src & pair.first) {
                flags |= pair.second;
            }
        }
        return flags;
    }

    inline vk::ImageAspectFlags GetAspectMask(TextureAspect aspect)
    {
        static std::unordered_map<TextureAspect, vk::ImageAspectFlags> rules = {
            { TextureAspect::color, vk::ImageAspectFlagBits::eColor },
            { TextureAspect::depth, vk::ImageAspectFlagBits::eDepth},
            { TextureAspect::stencil, vk::ImageAspectFlagBits::eStencil },
            { TextureAspect::depthStencil, vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil }
        };

        vk::ImageAspectFlags result = {};
        for (const auto& rule : rules) {
            if (aspect == rule.first) {
                result = rule.second;
            }
        }
        return result;
    }
}
