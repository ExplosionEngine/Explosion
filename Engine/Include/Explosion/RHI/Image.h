//
// Created by Administrator on 2021/4/23 0023.
//

#ifndef EXPLOSION_IMAGE_H
#define EXPLOSION_IMAGE_H

#include <vulkan/vulkan.h>

#include <Explosion/RHI/Enum.h>

namespace Explosion {
    class Driver;
    class Device;

    class Image : public GpuRes {
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

        Image(Driver& driver, const Config& config);
        explicit Image(Driver& driver, const VkImage& vkImage, const Config& config);
        ~Image() override;

        const VkImage& GetVkImage();
        const Config& GetConfig();

    protected:
        void OnCreate() override;
        void OnDestroy() override;

        Device& device;

    private:
        void CreateImage();
        void DestroyImage();

        bool fromSwapChain = false;
        VkImage vkImage = VK_NULL_HANDLE;
        Config config {};
    };
}

#endif //EXPLOSION_IMAGE_H
