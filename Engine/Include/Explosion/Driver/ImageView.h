//
// Created by John Kindem on 2021/4/24.
//

#ifndef EXPLOSION_IMAGEVIEW_H
#define EXPLOSION_IMAGEVIEW_H

#include <vulkan/vulkan.h>

#include <Explosion/Driver/Enum.h>

namespace Explosion {
    class Driver;
    class Device;
    class Image;

    class ImageView {
    public:
        struct Config {
            ImageViewType type;
            uint32_t mipLevelCount;
            uint32_t baseMipLevel;
            uint32_t layerCount;
            uint32_t baseLayer;
        };

        ImageView(Driver& driver, Image* image, const Config& config);
        ~ImageView();
        Image* GetImage();
        const VkImageView& GetVkImageView();

    private:
        void CreateImageView();
        void DestroyImageView();

        Driver& driver;
        Device& device;
        Config config {};
        Image* image = nullptr;
        VkImageView vkImageView = VK_NULL_HANDLE;
    };
}

#endif //EXPLOSION_IMAGEVIEW_H
