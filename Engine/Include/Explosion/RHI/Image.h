//
// Created by Administrator on 2021/4/23 0023.
//

#ifndef EXPLOSION_IMAGE_H
#define EXPLOSION_IMAGE_H

#include <vulkan/vulkan.h>

#include <Explosion/RHI/Enum.h>

namespace Explosion::RHI {
    class Driver;
    class Device;

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
            std::vector<ImageUsage> usages;
        };

        Image(Driver& driver, Config config);
        explicit Image(Driver& driver, const VkImage& vkImage, Config config);
        ~Image();

        const VkImage& GetVkImage();
        const Config& GetConfig();

    private:
        void CreateImage();
        void DestroyImage();

        Driver& driver;
        Device& device;
        Config config {};
        bool fromSwapChain = false;
        VkImage vkImage = VK_NULL_HANDLE;
    };
}

#endif //EXPLOSION_IMAGE_H
