//
// Created by johnk on 5/1/2022.
//

#ifndef EXPLOSION_RHI_IMAGE_VIEW_H
#define EXPLOSION_RHI_IMAGE_VIEW_H

#include <Common/Utility.h>
#include <RHI/Enum.h>

namespace RHI {
    class Image;

    struct ComponentMapping {
        ComponentSwizzle r;
        ComponentSwizzle g;
        ComponentSwizzle b;
        ComponentSwizzle a;
    };

    struct SubResourceRange {
        ImageAspectFlags aspect;
        size_t baseMipLevel;
        size_t levelCount;
        size_t baseArrayLayer;
        size_t layerCount;
    };

    struct ImageViewCreateInfo {
        Image* image;
        ImageViewType type;
        PixelFormat format;
        ComponentMapping componentMapping;
        SubResourceRange subResourceRange;
    };

    class ImageView {
    public:
        NON_COPYABLE(ImageView)
        virtual ~ImageView();

    protected:
        explicit ImageView(const ImageViewCreateInfo* createInfo);
    };
}

#endif //EXPLOSION_RHI_IMAGE_VIEW_H
