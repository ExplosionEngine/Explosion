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

    enum class ImageType {
        IMAGE_1D = 0,
        IMAGE_2D,
        IMAGE_3D,
        MAX
    };

    enum class ImageViewType {
        IMAGE_VIEW_1D = 0,
        IMAGE_VIEW_2D,
        IMAGE_VIEW_3D,
        IMAGE_VIEW_CUBE,
        IMAGE_VIEW_1D_ARRAY,
        IMAGE_VIEW_2D_ARRAY,
        IMAGE_VIEW_CUBE_ARRAY,
        MAX
    };

    enum class ComponentSwizzle {
        IDENTITY = 0,
        ZERO,
        ONE,
        R,
        G,
        B,
        A,
        MAX
    };

    enum class ShaderStage {
        VERTEX = 0,
        FRAGMENT,
        MAX
    };
}

namespace RHI {
    using Flags = uint64_t;

    enum class BufferUsageBits {
        NONE = 0x0,
        TRANSFER_SRC = 0x1,
        TRANSFER_DST = 0x2,
        VERTEX = 0x4,
        INDEX = 0x8,
        MAX
    };
    using BufferUsageFlags = Flags;

    enum class ImageUsageBits {
        NONE = 0x0,
        TRANSFER_SRC = 0x1,
        TRANSFER_DST = 0x2,
        SAMPLED = 0x4,
        MAX
    };
    using ImageUsageFlags = Flags;

    enum class ImageAspectBits {
        NONE = 0x0,
        COLOR = 0x1,
        DEPTH = 0x2,
        STENCIL = 0x4,
        MAX
    };
    using ImageAspectFlags = Flags;

    enum class ShaderCompileBits {
        NONE = 0x0,
        DEBUG = 0x1,
        NO_OPT = 0x2,
        MAX
    };
    using ShaderCompileFlags = Flags;

    template <typename... E>
    Flags CombineBits(E&&... e)
    {
        Flags result;
        std::initializer_list<int> { (result |= static_cast<Flags>(e), 0)... };
        return result;
    }
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
