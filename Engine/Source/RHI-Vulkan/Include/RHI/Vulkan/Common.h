//
// Created by johnk on 11/1/2022.
//

#pragma once

#include <Common/Debug.h>
#include <RHI/Common.h>
#include <unordered_map>
#include <vulkan/vulkan.h>

#define VK_KHRONOS_VALIDATION_LAYER_NAME "VK_LAYER_KHRONOS_validation"

#define VK_ECIMPL_BEGIN(A, B) template <> inline B VKEnumCast<A, B>(const A& value) {
#define VK_ECIMPL_ITEM(A, B) if (value == A) { return B; }
#define VK_ECIMPL_END(B) Unimplement(); return (B) 0; };

// duplicated code because static variable and namespace
namespace RHI::Vulkan {
    template <typename A, typename B>
    static const std::unordered_map<A, B> VK_ENUM_MAP;

    template <typename A, typename B>
    inline B VKEnumCast(const A& value);
}

// enum map definitions
namespace RHI::Vulkan {
    VK_ECIMPL_BEGIN(VkPhysicalDeviceType, GpuType)
        VK_ECIMPL_ITEM(VK_PHYSICAL_DEVICE_TYPE_OTHER,          GpuType::software)
        VK_ECIMPL_ITEM(VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU, GpuType::hardware)
        VK_ECIMPL_ITEM(VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,   GpuType::hardware)
        VK_ECIMPL_ITEM(VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU,    GpuType::software)
        VK_ECIMPL_ITEM(VK_PHYSICAL_DEVICE_TYPE_CPU,            GpuType::software)
    VK_ECIMPL_END(GpuType)

    VK_ECIMPL_BEGIN(PixelFormat, VkFormat)
        // 8-Bits
        VK_ECIMPL_ITEM(PixelFormat::r8Unorm, VK_FORMAT_R8_UNORM)
        VK_ECIMPL_ITEM(PixelFormat::r8Snorm, VK_FORMAT_R8_SNORM)
        VK_ECIMPL_ITEM(PixelFormat::r8Uint,  VK_FORMAT_R8_UINT)
        VK_ECIMPL_ITEM(PixelFormat::r8Sint,  VK_FORMAT_R8_SINT)
        // 16-Bits
        VK_ECIMPL_ITEM(PixelFormat::r16Uint,  VK_FORMAT_R16_UINT)
        VK_ECIMPL_ITEM(PixelFormat::r16Sint,  VK_FORMAT_R16_SINT)
        VK_ECIMPL_ITEM(PixelFormat::r16Float, VK_FORMAT_R16_SFLOAT)
        VK_ECIMPL_ITEM(PixelFormat::rg8Unorm, VK_FORMAT_R8G8_UNORM)
        VK_ECIMPL_ITEM(PixelFormat::rg8Snorm, VK_FORMAT_R8G8_SNORM)
        VK_ECIMPL_ITEM(PixelFormat::rg8Uint,  VK_FORMAT_R8G8_UINT)
        VK_ECIMPL_ITEM(PixelFormat::rg8Sint,  VK_FORMAT_R8G8_SINT)
        // 32-Bits
        VK_ECIMPL_ITEM(PixelFormat::r32Uint,         VK_FORMAT_R32_UINT)
        VK_ECIMPL_ITEM(PixelFormat::r32Sint,         VK_FORMAT_R32_SINT)
        VK_ECIMPL_ITEM(PixelFormat::r32Float,        VK_FORMAT_R32_SFLOAT)
        VK_ECIMPL_ITEM(PixelFormat::rg16Uint,        VK_FORMAT_R16G16_UINT)
        VK_ECIMPL_ITEM(PixelFormat::rg16Sint,        VK_FORMAT_R16G16_SINT)
        VK_ECIMPL_ITEM(PixelFormat::rg16Float,       VK_FORMAT_R16G16_SFLOAT)
        VK_ECIMPL_ITEM(PixelFormat::rgba8Unorm,      VK_FORMAT_R8G8B8A8_UNORM)
        VK_ECIMPL_ITEM(PixelFormat::rgba8UnormSrgb,  VK_FORMAT_R8G8B8A8_SRGB)
        VK_ECIMPL_ITEM(PixelFormat::rgba8Snorm,      VK_FORMAT_R8G8B8A8_SNORM)
        VK_ECIMPL_ITEM(PixelFormat::rgba8Uint,       VK_FORMAT_R8G8B8A8_UINT)
        VK_ECIMPL_ITEM(PixelFormat::rgba8Sint,       VK_FORMAT_R8G8B8A8_SINT)
        VK_ECIMPL_ITEM(PixelFormat::bgra8Unorm,      VK_FORMAT_B8G8R8A8_UNORM)
        VK_ECIMPL_ITEM(PixelFormat::bgra8UnormSrgb,  VK_FORMAT_B8G8R8A8_SRGB)
        VK_ECIMPL_ITEM(PixelFormat::rgb9E5Float,     VK_FORMAT_E5B9G9R9_UFLOAT_PACK32)
        VK_ECIMPL_ITEM(PixelFormat::rgb10A2Unorm,    VK_FORMAT_A2R10G10B10_UNORM_PACK32)
        VK_ECIMPL_ITEM(PixelFormat::rg11B10Float,    VK_FORMAT_B10G11R11_UFLOAT_PACK32)
        // 64-Bits
        VK_ECIMPL_ITEM(PixelFormat::rg32Uint,        VK_FORMAT_R32G32_UINT)
        VK_ECIMPL_ITEM(PixelFormat::rg32Sint,        VK_FORMAT_R32G32_SINT)
        VK_ECIMPL_ITEM(PixelFormat::rg32Float,       VK_FORMAT_R32G32_SFLOAT)
        VK_ECIMPL_ITEM(PixelFormat::rgba16Uint,      VK_FORMAT_R16G16B16A16_UINT)
        VK_ECIMPL_ITEM(PixelFormat::rgba16Sint,      VK_FORMAT_R16G16B16A16_SINT)
        VK_ECIMPL_ITEM(PixelFormat::rgba16Float,     VK_FORMAT_R16G16B16A16_SFLOAT)
        // 128-Bits
        VK_ECIMPL_ITEM(PixelFormat::rgba32Uint,      VK_FORMAT_R32G32B32A32_UINT)
        VK_ECIMPL_ITEM(PixelFormat::rgba32Sint,      VK_FORMAT_R32G32B32A32_SINT)
        VK_ECIMPL_ITEM(PixelFormat::rgba32Float,     VK_FORMAT_R32G32B32A32_SFLOAT)
        // Depth-Stencil
        VK_ECIMPL_ITEM(PixelFormat::d16Unorm,        VK_FORMAT_D16_UNORM)
        VK_ECIMPL_ITEM(PixelFormat::d24UnormS8Uint,  VK_FORMAT_D24_UNORM_S8_UINT)
        VK_ECIMPL_ITEM(PixelFormat::d32Float,        VK_FORMAT_D32_SFLOAT)
        VK_ECIMPL_ITEM(PixelFormat::d32FloatS8Uint,  VK_FORMAT_D32_SFLOAT_S8_UINT)
        // Undefined
        VK_ECIMPL_ITEM(PixelFormat::max,             VK_FORMAT_UNDEFINED)
    VK_ECIMPL_END(VkFormat)

    VK_ECIMPL_BEGIN(QueueType, VkQueueFlagBits)
        VK_ECIMPL_ITEM(QueueType::graphics, VK_QUEUE_GRAPHICS_BIT)
        VK_ECIMPL_ITEM(QueueType::compute,  VK_QUEUE_COMPUTE_BIT)
        VK_ECIMPL_ITEM(QueueType::transfer, VK_QUEUE_TRANSFER_BIT)
    VK_ECIMPL_END(VkQueueFlagBits)

    VK_ECIMPL_BEGIN(TextureDimension, VkImageType)
        VK_ECIMPL_ITEM(TextureDimension::t1D, VK_IMAGE_TYPE_1D)
        VK_ECIMPL_ITEM(TextureDimension::t2D, VK_IMAGE_TYPE_2D)
        VK_ECIMPL_ITEM(TextureDimension::t3D, VK_IMAGE_TYPE_3D)
    VK_ECIMPL_END(VkImageType)

    VK_ECIMPL_BEGIN(TextureViewDimension, VkImageViewType)
        VK_ECIMPL_ITEM(TextureViewDimension::tv1D,        VK_IMAGE_VIEW_TYPE_1D)
        VK_ECIMPL_ITEM(TextureViewDimension::tv2D,        VK_IMAGE_VIEW_TYPE_2D)
        VK_ECIMPL_ITEM(TextureViewDimension::tv2DArray,   VK_IMAGE_VIEW_TYPE_2D_ARRAY)
        VK_ECIMPL_ITEM(TextureViewDimension::tvCube,      VK_IMAGE_VIEW_TYPE_CUBE)
        VK_ECIMPL_ITEM(TextureViewDimension::tvCubeArray, VK_IMAGE_VIEW_TYPE_CUBE_ARRAY)
        VK_ECIMPL_ITEM(TextureViewDimension::tv3D,        VK_IMAGE_VIEW_TYPE_3D)
    VK_ECIMPL_END(VkImageViewType)

    VK_ECIMPL_BEGIN(ShaderStageBits, VkShaderStageFlagBits)
        VK_ECIMPL_ITEM(ShaderStageBits::sVertex,  VK_SHADER_STAGE_VERTEX_BIT)
        VK_ECIMPL_ITEM(ShaderStageBits::sPixel,   VK_SHADER_STAGE_FRAGMENT_BIT)
        VK_ECIMPL_ITEM(ShaderStageBits::sCompute, VK_SHADER_STAGE_COMPUTE_BIT)
    VK_ECIMPL_END(VkShaderStageFlagBits)

    VK_ECIMPL_BEGIN(PrimitiveTopologyType, VkPrimitiveTopology)
        VK_ECIMPL_ITEM(PrimitiveTopologyType::point,    VK_PRIMITIVE_TOPOLOGY_POINT_LIST)
        VK_ECIMPL_ITEM(PrimitiveTopologyType::line,     VK_PRIMITIVE_TOPOLOGY_LINE_LIST)
        VK_ECIMPL_ITEM(PrimitiveTopologyType::triangle, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
    VK_ECIMPL_END(VkPrimitiveTopology)

    VK_ECIMPL_BEGIN(CullMode, VkCullModeFlagBits)
        VK_ECIMPL_ITEM(CullMode::none,  VK_CULL_MODE_NONE)
        VK_ECIMPL_ITEM(CullMode::front, VK_CULL_MODE_FRONT_BIT)
        VK_ECIMPL_ITEM(CullMode::back,  VK_CULL_MODE_BACK_BIT)
    VK_ECIMPL_END(VkCullModeFlagBits)

    VK_ECIMPL_BEGIN(BlendOp, VkBlendOp)
        VK_ECIMPL_ITEM(BlendOp::opAdd,              VK_BLEND_OP_ADD)
        VK_ECIMPL_ITEM(BlendOp::opSubstract,        VK_BLEND_OP_SUBTRACT)
        VK_ECIMPL_ITEM(BlendOp::opReverseSubstract, VK_BLEND_OP_REVERSE_SUBTRACT)
        VK_ECIMPL_ITEM(BlendOp::opMin,              VK_BLEND_OP_MIN)
        VK_ECIMPL_ITEM(BlendOp::opMax,              VK_BLEND_OP_MAX)
    VK_ECIMPL_END(VkBlendOp)

    VK_ECIMPL_BEGIN(BlendFactor, VkBlendFactor)
        VK_ECIMPL_ITEM(BlendFactor::zero,             VK_BLEND_FACTOR_ZERO)
        VK_ECIMPL_ITEM(BlendFactor::one,              VK_BLEND_FACTOR_ONE)
        VK_ECIMPL_ITEM(BlendFactor::src,              VK_BLEND_FACTOR_SRC_COLOR)
        VK_ECIMPL_ITEM(BlendFactor::oneMinusSrc,      VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR)
        VK_ECIMPL_ITEM(BlendFactor::srcAlpha,         VK_BLEND_FACTOR_SRC_ALPHA)
        VK_ECIMPL_ITEM(BlendFactor::oneMinusSrcAlpha, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA)
        VK_ECIMPL_ITEM(BlendFactor::dst,              VK_BLEND_FACTOR_DST_COLOR)
        VK_ECIMPL_ITEM(BlendFactor::oneMinusDst,      VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR)
        VK_ECIMPL_ITEM(BlendFactor::dstAlpha,         VK_BLEND_FACTOR_DST_ALPHA)
        VK_ECIMPL_ITEM(BlendFactor::oneMinusDstAlpha, VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA)
    VK_ECIMPL_END(VkBlendFactor)

    VK_ECIMPL_BEGIN(VertexFormat, VkFormat)
        VK_ECIMPL_ITEM(VertexFormat::uint8X2,   VK_FORMAT_R8G8_UINT)
        VK_ECIMPL_ITEM(VertexFormat::uint8X4,   VK_FORMAT_R8G8B8A8_UINT)
        VK_ECIMPL_ITEM(VertexFormat::sint8X2,   VK_FORMAT_R8G8_SINT)
        VK_ECIMPL_ITEM(VertexFormat::sint8X4,   VK_FORMAT_R8G8B8A8_SINT)
        VK_ECIMPL_ITEM(VertexFormat::unorm8X2,  VK_FORMAT_R8G8_UNORM)
        VK_ECIMPL_ITEM(VertexFormat::unorm8X4,  VK_FORMAT_R8G8B8A8_UNORM)
        VK_ECIMPL_ITEM(VertexFormat::snorm8X2,  VK_FORMAT_R8G8_SNORM)
        VK_ECIMPL_ITEM(VertexFormat::snorm8X4,  VK_FORMAT_R8G8B8A8_SNORM)
        VK_ECIMPL_ITEM(VertexFormat::uint16X2,  VK_FORMAT_R16G16_UINT)
        VK_ECIMPL_ITEM(VertexFormat::uint16X4,  VK_FORMAT_R16G16B16A16_UINT)
        VK_ECIMPL_ITEM(VertexFormat::sint16X2,  VK_FORMAT_R16G16_SINT)
        VK_ECIMPL_ITEM(VertexFormat::sint16X4,  VK_FORMAT_R16G16B16A16_SINT)
        VK_ECIMPL_ITEM(VertexFormat::unorm16X2, VK_FORMAT_R16G16_UNORM)
        VK_ECIMPL_ITEM(VertexFormat::unorm16X4, VK_FORMAT_R16G16B16A16_UNORM)
        VK_ECIMPL_ITEM(VertexFormat::snorm16X2, VK_FORMAT_R16G16_SNORM)
        VK_ECIMPL_ITEM(VertexFormat::snorm16X4, VK_FORMAT_R16G16B16A16_SNORM)
        VK_ECIMPL_ITEM(VertexFormat::float16X2, VK_FORMAT_R16G16_SFLOAT)
        VK_ECIMPL_ITEM(VertexFormat::float16X4, VK_FORMAT_R16G16B16A16_SFLOAT)
        VK_ECIMPL_ITEM(VertexFormat::float32X1, VK_FORMAT_R32_SFLOAT)
        VK_ECIMPL_ITEM(VertexFormat::float32X2, VK_FORMAT_R32G32_SFLOAT)
        VK_ECIMPL_ITEM(VertexFormat::float32X3, VK_FORMAT_R32G32B32_SFLOAT)
        VK_ECIMPL_ITEM(VertexFormat::float32X4, VK_FORMAT_R32G32B32A32_SFLOAT)
        VK_ECIMPL_ITEM(VertexFormat::uint32X1,  VK_FORMAT_R32_UINT)
        VK_ECIMPL_ITEM(VertexFormat::uint32X2,  VK_FORMAT_R32G32_UINT)
        VK_ECIMPL_ITEM(VertexFormat::uint32X3,  VK_FORMAT_R32G32B32_UINT)
        VK_ECIMPL_ITEM(VertexFormat::uint32X4,  VK_FORMAT_R32G32B32A32_UINT)
        VK_ECIMPL_ITEM(VertexFormat::sint32X1,  VK_FORMAT_R32_SINT)
        VK_ECIMPL_ITEM(VertexFormat::sint32X2,  VK_FORMAT_R32G32_SINT)
        VK_ECIMPL_ITEM(VertexFormat::sint32X3,  VK_FORMAT_R32G32B32_SINT)
        VK_ECIMPL_ITEM(VertexFormat::sint32X4,  VK_FORMAT_R32G32B32A32_SINT)
    VK_ECIMPL_END(VkFormat)

    VK_ECIMPL_BEGIN(BindingType, VkDescriptorType)
        VK_ECIMPL_ITEM(BindingType::uniformBuffer,  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
        VK_ECIMPL_ITEM(BindingType::storageBuffer,  VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
        VK_ECIMPL_ITEM(BindingType::sampler,        VK_DESCRIPTOR_TYPE_SAMPLER)
        VK_ECIMPL_ITEM(BindingType::texture,        VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE)
        VK_ECIMPL_ITEM(BindingType::storageTexture, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE)
    VK_ECIMPL_END(VkDescriptorType)

    VK_ECIMPL_BEGIN(AddressMode, VkSamplerAddressMode)
        VK_ECIMPL_ITEM(AddressMode::clampToEdge,  VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE)
        VK_ECIMPL_ITEM(AddressMode::repeat,       VK_SAMPLER_ADDRESS_MODE_REPEAT)
        VK_ECIMPL_ITEM(AddressMode::mirrorRepeat, VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT)
    VK_ECIMPL_END(VkSamplerAddressMode)

    VK_ECIMPL_BEGIN(FilterMode, VkFilter)
        VK_ECIMPL_ITEM(FilterMode::nearest, VK_FILTER_NEAREST)
        VK_ECIMPL_ITEM(FilterMode::linear,  VK_FILTER_LINEAR)
    VK_ECIMPL_END(VkFilter)

    VK_ECIMPL_BEGIN(FilterMode, VkSamplerMipmapMode)
        VK_ECIMPL_ITEM(FilterMode::nearest, VK_SAMPLER_MIPMAP_MODE_NEAREST)
        VK_ECIMPL_ITEM(FilterMode::linear,  VK_SAMPLER_MIPMAP_MODE_LINEAR)
    VK_ECIMPL_END(VkSamplerMipmapMode)

    VK_ECIMPL_BEGIN(ComparisonFunc, VkCompareOp)
        VK_ECIMPL_ITEM(ComparisonFunc::never,        VK_COMPARE_OP_NEVER)
        VK_ECIMPL_ITEM(ComparisonFunc::less,         VK_COMPARE_OP_LESS)
        VK_ECIMPL_ITEM(ComparisonFunc::equal,        VK_COMPARE_OP_EQUAL)
        VK_ECIMPL_ITEM(ComparisonFunc::lessEqual,    VK_COMPARE_OP_LESS_OR_EQUAL)
        VK_ECIMPL_ITEM(ComparisonFunc::greater,      VK_COMPARE_OP_GREATER)
        VK_ECIMPL_ITEM(ComparisonFunc::notEqual,     VK_COMPARE_OP_NOT_EQUAL)
        VK_ECIMPL_ITEM(ComparisonFunc::greaterEqual, VK_COMPARE_OP_GREATER_OR_EQUAL)
        VK_ECIMPL_ITEM(ComparisonFunc::always,       VK_COMPARE_OP_ALWAYS)
    VK_ECIMPL_END(VkCompareOp)

    VK_ECIMPL_BEGIN(StencilOp, VkStencilOp)
        VK_ECIMPL_ITEM(StencilOp::keep,           VK_STENCIL_OP_KEEP)
        VK_ECIMPL_ITEM(StencilOp::zero,           VK_STENCIL_OP_ZERO)
        VK_ECIMPL_ITEM(StencilOp::replace,        VK_STENCIL_OP_REPLACE)
        VK_ECIMPL_ITEM(StencilOp::invert,         VK_STENCIL_OP_INVERT)
        VK_ECIMPL_ITEM(StencilOp::incrementClamp, VK_STENCIL_OP_INCREMENT_AND_CLAMP)
        VK_ECIMPL_ITEM(StencilOp::decrementClamp, VK_STENCIL_OP_DECREMENT_AND_CLAMP)
        VK_ECIMPL_ITEM(StencilOp::incrementWrap,  VK_STENCIL_OP_INCREMENT_AND_WRAP)
        VK_ECIMPL_ITEM(StencilOp::decrementWrap,  VK_STENCIL_OP_DECREMENT_AND_WRAP)
    VK_ECIMPL_END(VkStencilOp)

    VK_ECIMPL_BEGIN(LoadOp, VkAttachmentLoadOp)
        VK_ECIMPL_ITEM(LoadOp::load,  VK_ATTACHMENT_LOAD_OP_LOAD)
        VK_ECIMPL_ITEM(LoadOp::clear, VK_ATTACHMENT_LOAD_OP_CLEAR)
        VK_ECIMPL_ITEM(LoadOp::max,   VK_ATTACHMENT_LOAD_OP_NONE_EXT)
    VK_ECIMPL_END(VkAttachmentLoadOp)

    VK_ECIMPL_BEGIN(StoreOp, VkAttachmentStoreOp)
        VK_ECIMPL_ITEM(StoreOp::store,   VK_ATTACHMENT_STORE_OP_STORE)
        VK_ECIMPL_ITEM(StoreOp::discard, VK_ATTACHMENT_STORE_OP_DONT_CARE)
        VK_ECIMPL_ITEM(StoreOp::max,     VK_ATTACHMENT_STORE_OP_NONE_EXT)
    VK_ECIMPL_END(VkAttachmentStoreOp)

    VK_ECIMPL_BEGIN(IndexFormat, VkIndexType)
        VK_ECIMPL_ITEM(IndexFormat::uint16, VK_INDEX_TYPE_UINT16)
        VK_ECIMPL_ITEM(IndexFormat::uint32, VK_INDEX_TYPE_UINT32)
        VK_ECIMPL_ITEM(IndexFormat::max,    VK_INDEX_TYPE_NONE_KHR)
    VK_ECIMPL_END(VkIndexType)

    VK_ECIMPL_BEGIN(TextureState, VkImageLayout)
        VK_ECIMPL_ITEM(TextureState::undefined,    VK_IMAGE_LAYOUT_UNDEFINED)
        VK_ECIMPL_ITEM(TextureState::renderTarget, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
        VK_ECIMPL_ITEM(TextureState::present,      VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
        VK_ECIMPL_ITEM(TextureState::max,          VK_IMAGE_LAYOUT_GENERAL)
    VK_ECIMPL_END(VkImageLayout)

    VK_ECIMPL_BEGIN(PresentMode, VkPresentModeKHR)
        VK_ECIMPL_ITEM(PresentMode::immediately, VK_PRESENT_MODE_IMMEDIATE_KHR)
        VK_ECIMPL_ITEM(PresentMode::vsync,       VK_PRESENT_MODE_FIFO_KHR)
        VK_ECIMPL_ITEM(PresentMode::max,         VK_PRESENT_MODE_IMMEDIATE_KHR) // TODO Set the default present mode to immediate?
    VK_ECIMPL_END(VkPresentModeKHR)

    inline VkExtent3D FromRHI(const Common::UVec3& ext)
    {
        return { ext.x, ext.y, ext.z };
    }

    inline VkShaderStageFlags FromRHI(const ShaderStageFlags& src)
    {
        VkShaderStageFlags flags = {};
        for (auto& pair : VK_ENUM_MAP<ShaderStageBits, VkShaderStageFlagBits>) {
            if (src & pair.first) {
                flags |= pair.second;
            }
        }
        return flags;
    }

    inline VkImageAspectFlags GetAspectMask(TextureAspect aspect)
    {
        static std::unordered_map<TextureAspect, VkImageAspectFlags> rules = {
            { TextureAspect::color, VK_IMAGE_ASPECT_COLOR_BIT },
            { TextureAspect::depth, VK_IMAGE_ASPECT_DEPTH_BIT },
            { TextureAspect::stencil, VK_IMAGE_ASPECT_STENCIL_BIT },
            { TextureAspect::depthStencil, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT }
        };

        VkImageAspectFlags result = {};
        for (const auto& rule : rules) {
            if (aspect == rule.first) {
                result = rule.second;
            }
        }
        return result;
    }
}
