//
// Created by johnk on 4/1/2022.
//

#ifndef EXPLOSION_RHI_IMAGE_H
#define EXPLOSION_RHI_IMAGE_H

#include <Common/Utility.h>
#include <RHI/Enum.h>

namespace RHI {
    struct ImageCreateInfo {
        Extent2D extent;
        ImageType type;
        PixelFormat format;
        ImageUsageFlags usage;
        size_t mipLevels;
        size_t arrayLayers;
        size_t samples;
    };

    class Image {
    public:
        NON_COPYABLE(Image)
        virtual ~Image();

    protected:
        Image();
        explicit Image(const ImageCreateInfo* createInfo);
    };
}

#endif //EXPLOSION_RHI_IMAGE_H
