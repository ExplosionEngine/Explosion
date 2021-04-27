//
// Created by Administrator on 2021/4/23 0023.
//

#ifndef EXPLOSION_ENUM_H
#define EXPLOSION_ENUM_H

#include <unordered_map>

namespace Explosion {
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

    enum class ShaderStage {
        VERTEX,
        FRAGMENT,
        COMPUTE,
        MAX
    };

    enum class VertexInputRate {
        PER_VERTEX,
        PER_INSTANCE,
        MAX
    };

    enum class CullMode {
        NONE,
        FRONT,
        BACK,
        ALL,
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
        IMAGE_SAMPLER
    };
}

#endif //EXPLOSION_ENUM_H
