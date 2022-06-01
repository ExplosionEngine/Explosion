//
// Created by johnk on 2022/5/22.
//

#pragma once

#include <cstdint>

namespace Shader {
    using Float = float[1];
    using Float2 = float[2];
    using Float3 = float[3];
    using Float4 = float[4];

    using Int = int32_t[1];
    using Int2 = int32_t[2];
    using Int3 = int32_t[3];
    using Int4 = int32_t[4];

    using Uint = uint32_t[1];
    using Uint2 = uint32_t[2];
    using Uint3 = uint32_t[3];
    using Uint4 = uint32_t[4];

    template <typename T>
    struct Texture2D {};

    template <typename T>
    struct Texture3D {};

    template <typename T>
    struct RWTexture2D {};

    template <typename T>
    struct RWTexture3D {};

    struct SamplerState {};
}
