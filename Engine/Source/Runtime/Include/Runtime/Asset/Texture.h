//
// Created by johnk on 2023/4/29.
//

#pragma once

#include <cstdint>

#include <Runtime/Asset.h>

namespace Runtime {
    // less than pixel format
    enum class EEnum() TextureFormat {
        r8Unorm,
        r8Snorm,
        r8Uint,
        r16Uint,
        r16Float,
        rg8Unorm,
        rg8Snorm,
        rg8Uint,
        r32Uint,
        r32Float,
        rg16Uint,
        rg16Float,
        rgba8Unorm,
        rgba8UnormSrgb,
        rgba8Snorm,
        rgba8Uint,
        bgra8Unorm,
        bgra8UnormSrgb,
        rgb9E5Float,
        rgb10A2Unorm,
        rg11B10Float,
        rg32Uint,
        rg32Float,
        rgba16Uint,
        rgba16Float,
        rgba32Uint,
        rgba32Float,
        max
    };

    enum class EEnum() TextureDimension {
        dim1D,
        dim2D,
        dim2DArray,
        dimCube,
        dimCubeArray,
        dim3D,
        max
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
