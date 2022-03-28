//
// Created by johnk on 2022/1/23.
//

#pragma once

#include <Common/Utility.h>
#include <RHI/Enum.h>

namespace RHI {
    struct TextureViewCreateInfo;
    class TextureView;

    struct TextureCreateInfo {
        Extent<3> extent;
        uint8_t mipLevels;
        uint8_t samples;
        TextureDimension dimension;
        PixelFormat format;
        TextureUsageFlags usages;
    };

    class Texture {
    public:
        NON_COPYABLE(Texture)
        virtual ~Texture();

        virtual TextureView* CreateTextureView(const TextureViewCreateInfo* createInfo) = 0;
        virtual void Destroy() = 0;

    protected:
        Texture();
        explicit Texture(const TextureCreateInfo* createInfo);
    };
}
