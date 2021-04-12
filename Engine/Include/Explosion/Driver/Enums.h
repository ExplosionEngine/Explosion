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
}

#endif //EXPLOSION_ENUMS_H
