//
// Created by johnk on 2022/1/23.
//

#pragma once

#include <Common/Utility.h>
#include <RHI/Common.h>
#include <string>

namespace RHI {
    struct TextureViewCreateInfo;
    class TextureView;
    class Device;

    struct TextureCreateInfo {
        TextureDimension dimension;
        Extent<3> extent;
        PixelFormat format;
        TextureUsageFlags usages;
        uint8_t mipLevels;
        uint8_t samples;
        TextureState initialState;
        std::string debugName;
    };

    class Texture {
    public:
        NonCopyable(Texture)
        virtual ~Texture();

        virtual TextureView* CreateTextureView(const TextureViewCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;

    protected:
        Texture();
        explicit Texture(const TextureCreateInfo& createInfo);
    };
}
