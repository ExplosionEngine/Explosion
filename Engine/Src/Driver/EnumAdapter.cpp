//
// Created by John Kindem on 2021/4/24.
//

#include <vulkan/vulkan.h>

#include <Explosion/Driver/EnumAdapter.h>

namespace Explosion {
#define FIND_OR_EXCEPT                                            \
    auto iter = MAP.find(value);                                  \
    if (iter == MAP.end()) {                                      \
        throw std::runtime_error("found no suitable key in map"); \
    }                                                             \
    return iter->second;                                          \

#define VK_CONVERT_INSTANCE_B(Type, VkType)           \
    template <>                                       \
    VkType VkConvert<Type, VkType>(const Type& value) \
    {                                                 \

#define VK_CONVERT_INSTANCE_E \
    }                         \

#define GET_ENUM_BY_VK_INSTANCE_B(VkType, Type)         \
    template <>                                         \
    Type GetEnumByVk<VkType, Type>(const VkType& value) \
    {                                                   \

#define GET_ENUM_BY_VK_INSTANCE_E \
    }                             \

}

namespace Explosion {
    VK_CONVERT_INSTANCE_B(Format, VkFormat)
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
            { Format::R32_G32_B32_A32_FLOAT, VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT },
        };
        FIND_OR_EXCEPT
    VK_CONVERT_INSTANCE_E

    VK_CONVERT_INSTANCE_B(ImageType, VkImageType)
        static std::unordered_map<ImageType, VkImageType> MAP = {
            { ImageType::IMAGE_1D, VkImageType::VK_IMAGE_TYPE_1D },
            { ImageType::IMAGE_2D, VkImageType::VK_IMAGE_TYPE_2D },
            { ImageType::IMAGE_3D, VkImageType::VK_IMAGE_TYPE_3D }
        };
        FIND_OR_EXCEPT
    VK_CONVERT_INSTANCE_E

    VK_CONVERT_INSTANCE_B(ImageViewType, VkImageViewType)
        static std::unordered_map<ImageViewType, VkImageViewType> MAP = {
            { ImageViewType::VIEW_1D, VkImageViewType::VK_IMAGE_VIEW_TYPE_1D },
            { ImageViewType::VIEW_2D, VkImageViewType::VK_IMAGE_VIEW_TYPE_2D },
            { ImageViewType::VIEW_3D, VkImageViewType::VK_IMAGE_VIEW_TYPE_3D },
            { ImageViewType::VIEW_CUBE, VkImageViewType::VK_IMAGE_VIEW_TYPE_CUBE },
            { ImageViewType::VIEW_1D_ARRAY, VkImageViewType::VK_IMAGE_VIEW_TYPE_1D_ARRAY },
            { ImageViewType::VIEW_2D_ARRAY, VkImageViewType::VK_IMAGE_VIEW_TYPE_2D_ARRAY },
            { ImageViewType::VIEW_CUBE_ARRAY, VkImageViewType::VK_IMAGE_VIEW_TYPE_CUBE_ARRAY }
        };
        FIND_OR_EXCEPT
    VK_CONVERT_INSTANCE_E

    VK_CONVERT_INSTANCE_B(AttachmentLoadOp, VkAttachmentLoadOp)
        static std::unordered_map<AttachmentLoadOp, VkAttachmentLoadOp> MAP = {
            { AttachmentLoadOp::NONE, VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE },
            { AttachmentLoadOp::CLEAR, VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR }
        };
        FIND_OR_EXCEPT
    VK_CONVERT_INSTANCE_E

    VK_CONVERT_INSTANCE_B(AttachmentStoreOp, VkAttachmentStoreOp)
        static std::unordered_map<AttachmentStoreOp, VkAttachmentStoreOp> MAP = {
            { AttachmentStoreOp::NONE, VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE },
            { AttachmentStoreOp::STORE, VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE }
        };
        FIND_OR_EXCEPT
    VK_CONVERT_INSTANCE_E

    GET_ENUM_BY_VK_INSTANCE_B(VkFormat, Format)
        static std::unordered_map<VkFormat, Format> MAP = {
            { VkFormat::VK_FORMAT_UNDEFINED, Format::UNDEFINED },
            { VkFormat::VK_FORMAT_R8G8B8A8_SRGB, Format::R8_G8_B8_A8_RGB },
            { VkFormat::VK_FORMAT_B8G8R8A8_SRGB, Format::B8_G8_R8_A8_RGB },
            { VkFormat::VK_FORMAT_R32_UINT, Format::R32_UNSIGNED_INT },
            { VkFormat::VK_FORMAT_R32_SINT, Format::R32_SIGNED_INT },
            { VkFormat::VK_FORMAT_R32_SFLOAT, Format::R32_FLOAT },
            { VkFormat::VK_FORMAT_R32G32_UINT, Format::R32_G32_UNSIGNED_INT },
            { VkFormat::VK_FORMAT_R32G32_SINT, Format::R32_G32_SIGNED_INT },
            { VkFormat::VK_FORMAT_R32G32_SFLOAT, Format::R32_G32_FLOAT },
            { VkFormat::VK_FORMAT_R32G32B32_UINT, Format::R32_G32_B32_UNSIGNED_INT },
            { VkFormat::VK_FORMAT_R32G32B32_SINT, Format::R32_G32_B32_SIGNED_INT },
            { VkFormat::VK_FORMAT_R32G32B32_SFLOAT, Format::R32_G32_B32_FLOAT },
            { VkFormat::VK_FORMAT_R32G32B32A32_UINT, Format::R32_G32_B32_A32_UNSIGNED_INT },
            { VkFormat::VK_FORMAT_R32G32B32A32_SINT, Format::R32_G32_B32_A32_SIGNED_INT },
            { VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT, Format::R32_G32_B32_A32_FLOAT },
        };
        FIND_OR_EXCEPT
    GET_ENUM_BY_VK_INSTANCE_E
}
