//
// Created by johnk on 1/1/2022.
//

#ifndef EXPLOSION_RHI_ENUM_H
#define EXPLOSION_RHI_ENUM_H

#include <string>

namespace RHI {
    const std::string RHI_INSTANCE_EXT_NAME_SURFACE = "EXT_INS_Surface";
    const std::string RHI_INSTANCE_EXT_NAME_WINDOWS_SURFACE = "EXT_INS_WindowsSurface";

    const std::string RHI_DEVICE_EXT_NAME_SWAP_CHAIN = "EXT_DVC_SwapChain";
}

namespace RHI {
    enum class QueueFamilyType {
        GRAPHICS = 0,
        SECONDARY,
        COMPUTE,
        COPY,
        MAX
    };

    enum class PixelFormat {
        R8G8B8A8_UNORM = 0,
        MAX
    };

    enum class DeviceMemoryType {
        HOST_VISIBLE = 0,
        DEVICE_LOCAL,
        MAX
    };

    enum class BufferUsage {
        TRANSFER_SRC = 0,
        TRANSFER_DST,
        VERTEX,
        INDEX,
        MAX
    };

    enum class ImageUsage {
        TRANSFER_SRC = 0,
        TRANSFER_DST,
        SAMPLED,
        MAX
    };

    enum class ImageType {
        IMAGE_1D = 0,
        IMAGE_2D,
        IMAGE_3D,
        MAX
    };
}

namespace RHI {
    struct Extent2D {
        size_t x;
        size_t y;
    };

    struct Extent3D {
        size_t x;
        size_t y;
        size_t z;
    };
}

#endif //EXPLOSION_RHI_ENUM_H
