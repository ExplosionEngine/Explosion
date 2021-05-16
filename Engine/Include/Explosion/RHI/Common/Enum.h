//
// Created by Administrator on 2021/4/23 0023.
//

#ifndef EXPLOSION_ENUM_H
#define EXPLOSION_ENUM_H

#include <unordered_map>

/**
 * common defines
 */
namespace Explosion::RHI {
    using Flags = uint32_t;

    template <typename E, typename T>
    using enable_if_enum_t = std::enable_if_t<std::is_enum_v<E>, T>;

    template <typename E>
    Flags operator|(enable_if_enum_t<E, const E&> l, enable_if_enum_t<E, const E&> r)
    {
        return static_cast<Flags>(l) | static_cast<Flags>(r);
    }

    template <typename E>
    Flags operator&(enable_if_enum_t<E, const E&> l, enable_if_enum_t<E, const E&> r)
    {
        return static_cast<Flags>(l) & static_cast<Flags>(r);
    }
}

/**
 * enum class
 */
namespace Explosion::RHI {
    enum class Format {
        UNDEFINED,
        R8_G8_B8_A8_RGB,
        B8_G8_R8_A8_RGB,
        R32_UNSIGNED_INT,
        R32_SIGNED_INT,
        R32_FLOAT,
        R32_G32_UNSIGNED_INT,
        R32_G32_SIGNED_INT,
        R32_G32_FLOAT,
        R32_G32_B32_UNSIGNED_INT,
        R32_G32_B32_SIGNED_INT,
        R32_G32_B32_FLOAT,
        R32_G32_B32_A32_UNSIGNED_INT,
        R32_G32_B32_A32_SIGNED_INT,
        R32_G32_B32_A32_FLOAT,
        MAX
    };

    enum class ImageType {
        IMAGE_1D,
        IMAGE_2D,
        IMAGE_3D,
        MAX
    };

    enum class ImageViewType {
        VIEW_1D,
        VIEW_2D,
        VIEW_3D,
        VIEW_CUBE,
        VIEW_1D_ARRAY,
        VIEW_2D_ARRAY,
        VIEW_CUBE_ARRAY,
        MAX
    };

    enum class AttachmentType {
        COLOR_ATTACHMENT,
        SWAP_CHAIN_COLOR_ATTACHMENT,
        DEPTH_STENCIL_ATTACHMENT,
        MAX
    };

    enum class AttachmentLoadOp {
        NONE,
        CLEAR,
        MAX
    };

    enum class AttachmentStoreOp {
        NONE,
        STORE,
        MAX
    };

    enum class VertexInputRate {
        PER_VERTEX,
        PER_INSTANCE,
        MAX
    };

    enum class FrontFace {
        COUNTER_CLOCK_WISE,
        CLOCK_WISE,
        MAX
    };

    enum class DescriptorType {
        UNIFORM_BUFFER,
        STORAGE_BUFFER,
        IMAGE_SAMPLER,
        MAX
    };
}

/**
 * flags
 */
namespace Explosion::RHI {
    enum class ShaderStageBits {
        VERTEX = 0x1,
        FRAGMENT = 0x2,
        COMPUTE = 0x4,
        MAX
    };
    using ShaderStageFlags = Flags;

    enum class CullModeBits {
        NONE = 0x1,
        FRONT = 0x2,
        BACK = 0x4,
        MAX
    };
    using CullModeFlags = Flags;

    enum class BufferUsage {
        VERTEX_BUFFER = 0x1,
        INDEX_BUFFER = 0x2,
        UNIFORM_BUFFER = 0x4,
        STORAGE_BUFFER = 0x8,
        TRANSFER_SRC = 0x10,
        TRANSFER_DST = 0x20,
        MAX
    };
    using BufferUsageFlags = Flags;

    enum class PipelineStageBits {
        COLOR_ATTACHMENT_OUTPUT = 0x1,
        MAX
    };
    using PipelineStageFlags = Flags;

    enum class MemoryPropertyBits {
        DEVICE_LOCAL = 0x1,
        HOST_VISIBLE = 0x2,
        HOST_COHERENT = 0x4,
        MAX
    };
    using MemoryPropertyFlags = Flags;

    enum class ImageUsageBits {
        TRANSFER_SRC = 0x1,
        TRANSFER_DST = 0x2,
        COLOR_ATTACHMENT = 0x3,
        DEPTH_STENCIL_ATTACHMENT = 0x4,
        MAX
    };
    using ImageUsageFlags = Flags;

    enum class ImageLayoutBits {
        UNDEFINED = 0x1,
        COLOR_ATTACHMENT_OPTIMAL = 0x2,
        DEPTH_STENCIL_ATTACHMENT_OPTIMAL = 0x4,
        TRANSFER_SRC_OPTIMAL = 0x8,
        TRANSFER_DST_OPTIMAL = 0x10,
        PRESENT_SRC = 0x20,
        MAX
    };
    using ImageLayoutFlags = Flags;

    enum class ImageAspectBits {
        COLOR = 0x1,
        DEPTH = 0x2,
        STENCIL = 0x4,
        MAX
    };
    using ImageAspectFlags = Flags;
}

#endif //EXPLOSION_ENUM_H
