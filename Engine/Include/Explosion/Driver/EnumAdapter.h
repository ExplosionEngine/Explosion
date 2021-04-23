//
// Created by Administrator on 2021/4/23 0023.
//

#ifndef EXPLOSION_ENUMADAPTER_H
#define EXPLOSION_ENUMADAPTER_H

#include <stdexcept>

#include <Explosion/Driver/Enum.h>

namespace Explosion {
#define CONVERT_INSTANCE_B(Type, VkType)              \
    template <>                                       \
    VkType VkConvert<Type, VkType>(const Type& value) \
    {                                                 \

#define CONVERT_INSTANCE_E \
    }                      \

#define FIND_OR_EXCEPT                                             \
    auto iter = MAP.find(value);                                   \
    if (iter == MAP.end()) {                                       \
        throw std::runtime_error("found no suitable vulkan enum"); \
    }                                                              \
    return iter->second;                                           \

}

namespace Explosion {
    template <typename Type, typename VkType>
    VkType VkConvert(const Type& type)
    {
        throw std::runtime_error("failed to find suitable template instance");
    }

    CONVERT_INSTANCE_B(Format, VkFormat)
        static std::unordered_map<Format, VkFormat> MAP = {
            { Format::UNDEFINED, VkFormat::VK_FORMAT_UNDEFINED },
            { Format::R8_G8_B8_A8_RGB, VkFormat::VK_FORMAT_R8G8B8A8_SRGB },
            { Format::B8_G8_R8_A8_RGB, VkFormat::VK_FORMAT_B8G8R8A8_SRGB },
            { Format::R32_UNSIGNED_INT, VkFormat::VK_FORMAT_R32_UINT },
            { Format::R32_SIGNED_INT, VkFormat::VK_FORMAT_R32_SINT },
            { Format::R32_FLOAT, VkFormat::VK_FORMAT_R32_SFLOAT },
            { Format::R32_G32_UNSIGNED_INT, VkFormat::VK_FORMAT_R32G32_UINT },
            { Format::R32_G32_SIGNED_INT, VkFormat::VK_FORMAT_R32G32_SINT },
            { Format::R32_G32_FLOAT, VkFormat::VK_FORMAT_R32G32_SFLOAT },
            { Format::R32_G32_B32_UNSIGNED_INT, VkFormat::VK_FORMAT_R32G32B32_UINT },
            { Format::R32_G32_B32_SIGNED_INT, VkFormat::VK_FORMAT_R32G32B32_SINT },
            { Format::R32_G32_B32_FLOAT, VkFormat::VK_FORMAT_R32G32B32_SFLOAT },
            { Format::R32_G32_B32_A32_UNSIGNED_INT, VkFormat::VK_FORMAT_R32G32B32A32_UINT },
            { Format::R32_G32_B32_A32_SIGNED_INT, VkFormat::VK_FORMAT_R32G32B32A32_SINT },
            { Format::R32_G32_B32_FLOAT, VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT },
        };
        FIND_OR_EXCEPT
    CONVERT_INSTANCE_E

    CONVERT_INSTANCE_B(ImageType, VkImageType)
        static std::unordered_map<ImageType, VkImageType> MAP = {
            { ImageType::IMAGE_1D, VkImageType::VK_IMAGE_TYPE_1D },
            { ImageType::IMAGE_2D, VkImageType::VK_IMAGE_TYPE_2D },
            { ImageType::IMAGE_3D, VkImageType::VK_IMAGE_TYPE_3D }
        };
        FIND_OR_EXCEPT
    CONVERT_INSTANCE_E

    CONVERT_INSTANCE_B(ImageUsage, VkImageUsageFlagBits)
        static std::unordered_map<ImageUsage, VkImageUsageFlagBits> MAP = {
            { ImageUsage::TRANSFER_SRC, VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT },
            { ImageUsage::TRANSFER_DST, VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT },
            { ImageUsage::COLOR_ATTACHMENT, VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT },
            { ImageUsage::DEPTH_STENCIL_ATTACHMENT, VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT }
        };
        FIND_OR_EXCEPT
    CONVERT_INSTANCE_E
}

#endif //EXPLOSION_ENUMADAPTER_H
