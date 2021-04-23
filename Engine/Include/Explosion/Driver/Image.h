//
// Created by Administrator on 2021/4/23 0023.
//

#ifndef EXPLOSION_IMAGE_H
#define EXPLOSION_IMAGE_H

#include <vulkan/vulkan.h>

namespace Explosion {
    class Device;

    class Image {
    public:
        struct Config {
            // TODO
        };

        Image(Device& device, Config config);
        explicit Image(VkImage& vkImage, Config config);
        ~Image();

        const VkImage& GetVkImage();
        const Config& GetConfig();

    private:
        void CreateImage();
        void DestroyImage();

        bool fromSwapChain = false;
        VkImage vkImage = VK_NULL_HANDLE;
        Config config {};
    };
}

#endif //EXPLOSION_IMAGE_H
