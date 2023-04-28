//
// Created by johnk on 2023/4/29.
//

#pragma once

#include <cstdint>

#include <Runtime/Asset.h>

namespace Runtime {
    // less than pixel format
    enum class EEnum() TextureFormat {
        R8_UNORM,
        R8_SNORM,
        R8_UINT,
        R16_UINT,
        R16_FLOAT,
        RG8_UNORM,
        RG8_SNORM,
        RG8_UINT,
        R32_UINT,
        R32_FLOAT,
        RG16_UINT,
        RG16_FLOAT,
        RGBA8_UNORM,
        RGBA8_UNORM_SRGB,
        RGBA8_SNORM,
        RGBA8_UINT,
        BGRA8_UNORM,
        BGRA8_UNORM_SRGB,
        RGB9_E5_FLOAT,
        RGB10A2_UNORM,
        RG11B10_FLOAT,
        RG32_UINT,
        RG32_FLOAT,
        RGBA16_UINT,
        RGBA16_FLOAT,
        RGBA32_UINT,
        RGBA32_FLOAT,
        MAX
    };

    enum class EEnum() TextureDimension {
        DIM_1D,
        DIM_2D,
        DIM_2D_ARRAY,
        DIM_CUBE,
        DIM_CUBE_ARRAY,
        DIM_3D,
        MAX
    };

    class EClass() Texture : public Asset {
    public:
        EProperty()
        TextureFormat format;

        EProperty(editorHide)
        std::vector<uint8_t> data;

        // TODO render proxy (transient, editor hide)
    };

    class EClass() Texture1D : public Texture {
    public:
        EProperty()
        size_t length;
    };

    class EClass() Texture2D : public Texture {
    public:
        EProperty()
        size_t width;

        EProperty()
        size_t height;
    };

    class EClass() Texture2DArray : public Texture {
    public:
        EProperty()
        size_t width;

        EProperty()
        size_t height;

        EProperty()
        size_t arraySize;
    };

    class EClass() TextureCube : public Texture {
    public:
        EProperty()
        size_t width;

        EProperty()
        size_t height;
    };

    class EClass() TextureCubeArray : public Texture {
    public:
        EProperty()
        size_t width;

        EProperty()
        size_t height;

        EProperty()
        size_t arraySize;
    };

    class EClass() Texture3D : public Texture {
    public:
        EProperty()
        size_t width;

        EProperty()
        size_t height;

        EProperty()
        size_t depth;
    };
}
