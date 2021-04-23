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

    enum class ImageUsage {
        TRANSFER_SRC,
        TRANSFER_DST,
        COLOR_ATTACHMENT,
        DEPTH_STENCIL_ATTACHMENT,
        MAX
    };
}

#endif //EXPLOSION_ENUM_H
