//
// Created by John Kindem on 2021/5/16 0016.
//

#ifndef EXPLOSION_IMAGEVIEW_H
#define EXPLOSION_IMAGEVIEW_H

#include <Explosion/RHI/Common/Enum.h>

namespace Explosion::RHI {
    class Image;

    class ImageView {
    public:
        struct Config {
            Image* image;
            ImageViewType type;
            ImageAspectFlags aspects;
            uint32_t mipLevelCount;
            uint32_t baseMipLevel;
            uint32_t layerCount;
            uint32_t baseLayer;
        };

        virtual ~ImageView();
        virtual Image* GetImage() = 0;

    protected:
        explicit ImageView(Config config);

        Config config;
    };
}

#endif //EXPLOSION_IMAGEVIEW_H
