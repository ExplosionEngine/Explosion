//
// Created by John Kindem on 2021/4/24.
//

#ifndef EXPLOSION_IMAGEVIEW_H
#define EXPLOSION_IMAGEVIEW_H

#include <vulkan/vulkan.h>

#include <Explosion/Driver/Enum.h>
#include <Explosion/Driver/GpuRes.h>

namespace Explosion {
    class Driver;
    class Device;
    class Image;

    class ImageView : public GpuRes {
    public:
        struct Config {
            Image* image;
            ImageViewType type;
            uint32_t mipLevelCount;
            uint32_t baseMipLevel;
            uint32_t layerCount;
            uint32_t baseLayer;
        };

        ImageView(Driver& driver, const Config& config);
        ~ImageView() override;
        Image* GetImage() const;
        const VkImageView& GetVkImageView();

    protected:
        void OnCreate() override;
        void OnDestroy() override;

    private:
        void CreateImageView();
        void DestroyImageView();

        Device& device;
        Config config {};
        VkImageView vkImageView = VK_NULL_HANDLE;
    };
}

#endif //EXPLOSION_IMAGEVIEW_H
