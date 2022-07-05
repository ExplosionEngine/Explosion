//
// Created by johnk on 2022/5/22.
//

#pragma once

#include <cstdint>

#include <RHI/Sampler.h>
#include <RHI/BufferView.h>
#include <RHI/TextureView.h>

namespace Shader {
    struct SamplerState {
        RHI::Sampler* sampler;
    };

    struct Buffer {
        RHI::BufferView* bufferView;
    };

    template <typename T, uint8_t>
    struct Texture {
        RHI::TextureView* textureView;
    };

    template <typename T, uint8_t>
    struct RWTexture {
        RHI::TextureView* textureView;
    };
}
