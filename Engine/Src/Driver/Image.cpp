//
// Created by Administrator on 2021/4/23 0023.
//

#include <Explosion/Driver/Image.h>
#include <Explosion/Driver/EnumAdapter.h>

namespace Explosion {
    Image::Image(Device& device, Image::Config config)
        : fromSwapChain(false), config(config)
    {
        CreateImage();
    }

    Image::Image(VkImage& vkImage, Image::Config config)
        : fromSwapChain(true), vkImage(vkImage), config(config) {}

    Image::~Image()
    {
        if (fromSwapChain) {
            return;
        }
        DestroyImage();
    }

    const VkImage& Image::GetVkImage()
    {
        return vkImage;
    }

    const Image::Config& Image::GetConfig()
    {
        return config;
    }

    void Image::CreateImage()
    {
        VkImageCreateInfo createInfo {};
        // TODO
    }

    void Image::DestroyImage()
    {
        // TODO
    }
}
