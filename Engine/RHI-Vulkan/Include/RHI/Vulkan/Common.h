//
// Created by johnk on 11/1/2022.
//

#ifndef EXPLOSION_RHI_VULKAN_H
#define EXPLOSION_RHI_VULKAN_H

#include <stdexcept>
#include <unordered_map>

#include <vulkan/vulkan.hpp>

#include <RHI/Enum.h>

namespace RHI::Vulkan {
    class VKException : public std::exception {
    public:
        explicit VKException(std::string m) : msg(std::move(m)) {}
        ~VKException() override = default;

        [[nodiscard]] const char* what() const noexcept override
        {
            return msg.c_str();
        }

    private:
        std::string msg;
    };
}

namespace RHI::Vulkan {
#if PLATFORM_WINDOWS
#define VK_KHR_PLATFORM_SURFACE_EXTENSION_NAME "VK_KHR_win32_surface"
#endif
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
        if (iter == VK_ENUM_MAP<A, B>.end()) {
            throw VKException("failed to find suitable enum cast");
        }
        return static_cast<B>(iter->second);
    }

#define VK_ENUM_MAP_BEGIN(A, B) template <> static const std::unordered_map<A, B> VK_ENUM_MAP<A, B> = {
#define VK_ENUM_MAP_ITEM(A, B) { A, B },
#define VK_ENUM_MAP_END() };
}

// enum map definitions
namespace RHI::Vulkan {
    VK_ENUM_MAP_BEGIN(vk::PhysicalDeviceType, GpuType)
        VK_ENUM_MAP_ITEM(vk::PhysicalDeviceType::eOther,         GpuType::SOFTWARE)
        VK_ENUM_MAP_ITEM(vk::PhysicalDeviceType::eIntegratedGpu, GpuType::HARDWARE)
        VK_ENUM_MAP_ITEM(vk::PhysicalDeviceType::eDiscreteGpu,   GpuType::HARDWARE)
        VK_ENUM_MAP_ITEM(vk::PhysicalDeviceType::eVirtualGpu,    GpuType::SOFTWARE)
        VK_ENUM_MAP_ITEM(vk::PhysicalDeviceType::eCpu,           GpuType::SOFTWARE)
    VK_ENUM_MAP_END()

    VK_ENUM_MAP_BEGIN(PixelFormat, vk::Format)
    // 8-Bits
        VK_ENUM_MAP_ITEM(PixelFormat::R8_UNORM, vk::Format::eR8Unorm)
        VK_ENUM_MAP_ITEM(PixelFormat::R8_SNORM, vk::Format::eR8Snorm)
        VK_ENUM_MAP_ITEM(PixelFormat::R8_UINT,  vk::Format::eR8Uint)
        VK_ENUM_MAP_ITEM(PixelFormat::R8_SINT,  vk::Format::eR8Sint)
        // 16-Bits
        VK_ENUM_MAP_ITEM(PixelFormat::R16_UINT,  vk::Format::eR16Uint)
        VK_ENUM_MAP_ITEM(PixelFormat::R16_SINT,  vk::Format::eR16Sint)
        VK_ENUM_MAP_ITEM(PixelFormat::R16_FLOAT, vk::Format::eR16Sfloat)
        VK_ENUM_MAP_ITEM(PixelFormat::RG8_UNORM, vk::Format::eR8G8Unorm)
        VK_ENUM_MAP_ITEM(PixelFormat::RG8_SNORM, vk::Format::eR8G8Snorm)
        VK_ENUM_MAP_ITEM(PixelFormat::RG8_UINT,  vk::Format::eR8G8Uint)
        VK_ENUM_MAP_ITEM(PixelFormat::RG8_SINT,  vk::Format::eR8G8Sint)
        // 32-Bits
        VK_ENUM_MAP_ITEM(PixelFormat::R32_UINT,         vk::Format::eR32Uint)
        VK_ENUM_MAP_ITEM(PixelFormat::R32_SINT,         vk::Format::eR32Sint)
        VK_ENUM_MAP_ITEM(PixelFormat::R32_FLOAT,        vk::Format::eR32Sfloat)
        VK_ENUM_MAP_ITEM(PixelFormat::RG16_UINT,        vk::Format::eR16G16Uint)
        VK_ENUM_MAP_ITEM(PixelFormat::RG16_SINT,        vk::Format::eR16G16Sint)
        VK_ENUM_MAP_ITEM(PixelFormat::RG16_FLOAT,       vk::Format::eR16G16Sfloat)
        VK_ENUM_MAP_ITEM(PixelFormat::RGBA8_UNORM,      vk::Format::eR8G8B8A8Unorm)
        VK_ENUM_MAP_ITEM(PixelFormat::RGBA8_UNORM_SRGB, vk::Format::eR8G8B8A8Srgb)
        VK_ENUM_MAP_ITEM(PixelFormat::RGBA8_SNORM,      vk::Format::eR8G8B8A8Snorm)
        VK_ENUM_MAP_ITEM(PixelFormat::RGBA8_UINT,       vk::Format::eR8G8B8A8Uint)
        VK_ENUM_MAP_ITEM(PixelFormat::RGBA8_SINT,       vk::Format::eR8G8B8A8Sint)
        VK_ENUM_MAP_ITEM(PixelFormat::BGRA8_UNORM,      vk::Format::eB8G8R8A8Unorm)
        VK_ENUM_MAP_ITEM(PixelFormat::BGRA8_UNORM_SRGB, vk::Format::eB8G8R8A8Srgb)
        VK_ENUM_MAP_ITEM(PixelFormat::RGB9_E5_FLOAT,    vk::Format::eE5B9G9R9UfloatPack32)
        VK_ENUM_MAP_ITEM(PixelFormat::RGB10A2_UNORM,    vk::Format::eA2R10G10B10UnormPack32)
        VK_ENUM_MAP_ITEM(PixelFormat::RG11B10_FLOAT,    vk::Format::eB10G11R11UfloatPack32)
        // 64-Bits
        VK_ENUM_MAP_ITEM(PixelFormat::RG32_UINT,        vk::Format::eR32G32Uint)
        VK_ENUM_MAP_ITEM(PixelFormat::RG32_SINT,        vk::Format::eR32G32Sint)
        VK_ENUM_MAP_ITEM(PixelFormat::RG32_FLOAT,       vk::Format::eR32G32Sfloat)
        VK_ENUM_MAP_ITEM(PixelFormat::RGBA16_UINT,      vk::Format::eR16G16B16A16Uint)
        VK_ENUM_MAP_ITEM(PixelFormat::RGBA16_SINT,      vk::Format::eR16G16B16A16Sint)
        VK_ENUM_MAP_ITEM(PixelFormat::RGBA16_FLOAT,     vk::Format::eR16G16B16A16Sfloat)
        // 128-Bits
        VK_ENUM_MAP_ITEM(PixelFormat::RGBA32_UINT,      vk::Format::eR32G32B32A32Uint)
        VK_ENUM_MAP_ITEM(PixelFormat::RGBA32_SINT,      vk::Format::eR32G32B32A32Sint)
        VK_ENUM_MAP_ITEM(PixelFormat::RGBA32_FLOAT,     vk::Format::eR32G32B32A32Sfloat)
        // Depth-Stencil
        VK_ENUM_MAP_ITEM(PixelFormat::D16_UNORM,         vk::Format::eD16Unorm)
        VK_ENUM_MAP_ITEM(PixelFormat::D24_UNORM_S8_UINT, vk::Format::eD24UnormS8Uint)
        VK_ENUM_MAP_ITEM(PixelFormat::D32_FLOAT,         vk::Format::eD32Sfloat)
    VK_ENUM_MAP_END()

    VK_ENUM_MAP_BEGIN(QueueType, vk::QueueFlagBits)
        VK_ENUM_MAP_ITEM(QueueType::GRAPHICS, vk::QueueFlagBits::eGraphics)
        VK_ENUM_MAP_ITEM(QueueType::COMPUTE,  vk::QueueFlagBits::eCompute)
        VK_ENUM_MAP_ITEM(QueueType::TRANSFER, vk::QueueFlagBits::eCompute)
    VK_ENUM_MAP_END()

    VK_ENUM_MAP_BEGIN(TextureDimension, vk::ImageType)
        VK_ENUM_MAP_ITEM(TextureDimension::T_1D, vk::ImageType::e1D)
        VK_ENUM_MAP_ITEM(TextureDimension::T_2D, vk::ImageType::e2D)
        VK_ENUM_MAP_ITEM(TextureDimension::T_3D, vk::ImageType::e3D)
    VK_ENUM_MAP_END()

    VK_ENUM_MAP_BEGIN(TextureViewDimension, vk::ImageViewType)
        VK_ENUM_MAP_ITEM(TextureViewDimension::TV_1D,         vk::ImageViewType::e1D)
        VK_ENUM_MAP_ITEM(TextureViewDimension::TV_2D,         vk::ImageViewType::e2D)
        VK_ENUM_MAP_ITEM(TextureViewDimension::TV_2D_ARRAY,   vk::ImageViewType::e2DArray)
        VK_ENUM_MAP_ITEM(TextureViewDimension::TV_CUBE,       vk::ImageViewType::eCube)
        VK_ENUM_MAP_ITEM(TextureViewDimension::TV_CUBE_ARRAY, vk::ImageViewType::eCubeArray)
        VK_ENUM_MAP_ITEM(TextureViewDimension::TV_3D,         vk::ImageViewType::e3D)
    VK_ENUM_MAP_END()

    VK_ENUM_MAP_BEGIN(ShaderStageBits, vk::ShaderStageFlagBits)
        VK_ENUM_MAP_ITEM(ShaderStageBits::VERTEX,   vk::ShaderStageFlagBits::eVertex)
        VK_ENUM_MAP_ITEM(ShaderStageBits::FRAGMENT, vk::ShaderStageFlagBits::eFragment)
        VK_ENUM_MAP_ITEM(ShaderStageBits::COMPUTE,  vk::ShaderStageFlagBits::eCompute)
    VK_ENUM_MAP_END()

    VK_ENUM_MAP_BEGIN(BindingType, vk::DescriptorType)
        VK_ENUM_MAP_ITEM(BindingType::UNIFORM_BUFFER,  vk::DescriptorType::eUniformBuffer)
        VK_ENUM_MAP_ITEM(BindingType::STORAGE_BUFFER,  vk::DescriptorType::eStorageBuffer)
        VK_ENUM_MAP_ITEM(BindingType::SAMPLER,         vk::DescriptorType::eCombinedImageSampler)
        VK_ENUM_MAP_ITEM(BindingType::TEXTURE,         vk::DescriptorType::eSampledImage)
        VK_ENUM_MAP_ITEM(BindingType::STORAGE_TEXTURE, vk::DescriptorType::eStorageImage)
    VK_ENUM_MAP_END()

    VK_ENUM_MAP_BEGIN(AddressMode, vk::SamplerAddressMode)
        VK_ENUM_MAP_ITEM(AddressMode::CLAMP_TO_EDGE, vk::SamplerAddressMode::eClampToEdge)
        VK_ENUM_MAP_ITEM(AddressMode::REPEAT,        vk::SamplerAddressMode::eRepeat)
        VK_ENUM_MAP_ITEM(AddressMode::MIRROR_REPEAT, vk::SamplerAddressMode::eMirroredRepeat)
    VK_ENUM_MAP_END()

    VK_ENUM_MAP_BEGIN(FilterMode, vk::Filter)
        VK_ENUM_MAP_ITEM(FilterMode::NEAREST, vk::Filter::eNearest)
        VK_ENUM_MAP_ITEM(FilterMode::LINEAR,  vk::Filter::eLinear)
    VK_ENUM_MAP_END()

    VK_ENUM_MAP_BEGIN(FilterMode, vk::SamplerMipmapMode)
        VK_ENUM_MAP_ITEM(FilterMode::NEAREST, vk::SamplerMipmapMode::eNearest)
        VK_ENUM_MAP_ITEM(FilterMode::LINEAR,  vk::SamplerMipmapMode::eLinear)
    VK_ENUM_MAP_END()

    VK_ENUM_MAP_BEGIN(ComparisonFunc, vk::CompareOp)
        VK_ENUM_MAP_ITEM(ComparisonFunc::NEVER,         vk::CompareOp::eNever)
        VK_ENUM_MAP_ITEM(ComparisonFunc::LESS,          vk::CompareOp::eLess)
        VK_ENUM_MAP_ITEM(ComparisonFunc::EQUAL,         vk::CompareOp::eEqual)
        VK_ENUM_MAP_ITEM(ComparisonFunc::LESS_EQUAL,    vk::CompareOp::eLessOrEqual)
        VK_ENUM_MAP_ITEM(ComparisonFunc::GREATER,       vk::CompareOp::eGreater)
        VK_ENUM_MAP_ITEM(ComparisonFunc::NOT_EQUAL,     vk::CompareOp::eNotEqual)
        VK_ENUM_MAP_ITEM(ComparisonFunc::GREATER_EQUAL, vk::CompareOp::eGreaterOrEqual)
        VK_ENUM_MAP_ITEM(ComparisonFunc::ALWAYS,        vk::CompareOp::eAlways)
    VK_ENUM_MAP_END()

    inline vk::Extent3D FromRHI(const RHI::Extent<3>& ext)
    {
        return { static_cast<uint32_t>(ext.x), static_cast<uint32_t>(ext.y), static_cast<uint32_t>(ext.z) };
    }
}

#endif //EXPLOSION_RHI_VULKAN_H
