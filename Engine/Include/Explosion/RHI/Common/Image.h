//
// Created by John Kindem on 2021/5/16 0016.
//

#ifndef EXPLOSION_IMAGE_H
#define EXPLOSION_IMAGE_H

#include <Explosion/RHI/Common/Enum.h>

namespace Explosion::RHI {
    class Image {
    public:
        struct Config {
            ImageType imageType;
            Format format;
            uint32_t width;
            uint32_t height;
            uint32_t depth;
            uint32_t mipLevels;
            uint32_t layers;
            ImageLayout initialLayout;
            BufferUsageFlags usages;
        };

        virtual ~Image();
        const Config& GetConfig();

    protected:
        explicit Image(Config config);

        Config config;
    };
}

#endif //EXPLOSION_IMAGE_H
