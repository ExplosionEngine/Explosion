//
// Created by John Kindem on 2021/4/10.
//

#ifndef EXPLOSION_IMAGE_H
#define EXPLOSION_IMAGE_H

#include <vulkan/vulkan.h>

namespace Explosion {
    class Image {
    public:
        explicit Image(const VkImage& vkImage);
        ~Image();

    private:
        bool holder = false;
        VkImage vkImage = VK_NULL_HANDLE;
    };
}

#endif //EXPLOSION_IMAGE_H
