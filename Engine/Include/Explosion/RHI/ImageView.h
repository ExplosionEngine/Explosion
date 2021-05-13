//
// Created by John Kindem on 2021/4/24.
//

#ifndef EXPLOSION_IMAGEVIEW_H
#define EXPLOSION_IMAGEVIEW_H

#include <vector>

#include <vulkan/vulkan.h>

#include <Explosion/RHI/Enum.h>

namespace Explosion {
    class Driver;
    class Device;
    class Image;

    class ImageView {
    public:
        struct Config {
            Image* image;
            ImageViewType type;
            std::vector<ImageAspect> aspects;
            uint32_t mipLevelCount;
            uint32_t baseMipLevel;
            uint32_t layerCount;
            uint32_t baseLayer;
        };

        ImageView(Driver& driver, Config config);
        ~ImageView();
        Image* GetImage();
        const VkImageView& GetVkImageView();

    private:
        void CreateImageView();
        void DestroyImageView();

        Driver& driver;
        Device& device;
        Config config {};
        VkImageView vkImageView = VK_NULL_HANDLE;
    };
}

#endif //EXPLOSION_IMAGEVIEW_H
