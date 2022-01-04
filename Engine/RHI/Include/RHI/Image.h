//
// Created by johnk on 4/1/2022.
//

#ifndef EXPLOSION_RHI_IMAGE_H
#define EXPLOSION_RHI_IMAGE_H

#include <Common/Utility.h>

namespace RHI {
    struct ImageCreateInfo {
        // TODO
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
