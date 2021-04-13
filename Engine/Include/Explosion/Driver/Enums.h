//
// Created by John Kindem on 2021/4/10.
//

#ifndef EXPLOSION_ENUMS_H
#define EXPLOSION_ENUMS_H

namespace Explosion {
    using Flags = uint32_t;

    template<typename Type, typename VkEnumType>
    VkEnumType VkConvert(const Type& type)
    {
        return static_cast<VkEnumType>(type);
    }

    template<typename Type>
    Flags GetFlags(const Type& type)
    {
        return static_cast<Flags>(type);
    }

    enum class Format {
        B8_G8_R8_A8_SRGB = VK_FORMAT_B8G8R8A8_SRGB,
        MAX
    };

    enum class ImageUsage {
        TRANSFER_SRC = VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
        TRANSFER_DST = VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        COLOR_ATTACHMENT = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        DEPTH_STENCIL_ATTACHMENT = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        MAX
    };
    using ImageUsageFlags = Flags;

    enum class ImageLayout {
        UNDEFINED = VK_IMAGE_LAYOUT_UNDEFINED,
        MAX
    };

    enum class VertexInputRate {
        VERTEX = VK_VERTEX_INPUT_RATE_VERTEX,
        INSTANCE = VK_VERTEX_INPUT_RATE_INSTANCE,
        MAX
    };

    enum class PrimitiveTopology {
        TRIANGLE_LIST = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        MAX
    };

    enum class PolygonMode {
        FILL = VK_POLYGON_MODE_FILL,
        LINE = VK_POLYGON_MODE_LINE,
        POINT = VK_POLYGON_MODE_POINT,
        MAX
    };

    enum class CullMode {
        FRONT = VK_CULL_MODE_FRONT_BIT,
        BACK = VK_CULL_MODE_BACK_BIT,
        MAX
    };
    using CullModeFlags = Flags;

    enum class FrontFace {
        CLOCKWISE = VK_FRONT_FACE_CLOCKWISE,
        MAX
    };

    enum class ImageType {
        I1D = VK_IMAGE_TYPE_1D,
        I2D = VK_IMAGE_TYPE_2D,
        I3D = VK_IMAGE_TYPE_3D,
        MAX
    };

    enum class ImageViewType {
        IV1D = VK_IMAGE_VIEW_TYPE_1D,
        IV2D = VK_IMAGE_VIEW_TYPE_2D,
        IV3D = VK_IMAGE_VIEW_TYPE_3D,
        CUBE = VK_IMAGE_VIEW_TYPE_CUBE,
        IV1D_ARRAY = VK_IMAGE_VIEW_TYPE_1D_ARRAY,
        IV2D_ARRAY = VK_IMAGE_VIEW_TYPE_2D_ARRAY,
    };
}

#endif //EXPLOSION_ENUMS_H
