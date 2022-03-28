//
// Created by Zach Lee on 2022/3/7.
//

#include <RHI/Vulkan/Texture.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Common.h>
#include <RHI/Vulkan/Gpu.h>
#include <unordered_map>

namespace RHI::Vulkan {
    static vk::ImageUsageFlags GetVkResourceStates(TextureUsageFlags textureUsages)
    {
        static std::unordered_map<TextureUsageBits, vk::ImageUsageFlags> rules = {
            { TextureUsageBits::COPY_SRC,          vk::ImageUsageFlagBits::eTransferSrc },
            { TextureUsageBits::COPY_DST,          vk::ImageUsageFlagBits::eTransferDst },
            { TextureUsageBits::TEXTURE_BINDING,   vk::ImageUsageFlagBits::eSampled },
            { TextureUsageBits::STORAGE_BINDING,   vk::ImageUsageFlagBits::eStorage },
            { TextureUsageBits::RENDER_ATTACHMENT, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eDepthStencilAttachment },
        };

        vk::ImageUsageFlags result = {};
        for (const auto& rule : rules) {
            if (textureUsages & rule.first) {
                result |= rule.second;
            }
        }
        return result;
    }

    VKTexture::VKTexture(VKDevice& dev, const TextureCreateInfo* createInfo)
        : Texture(createInfo), device(dev)
    {
        CreateImage(createInfo);
        AllocateMemory(createInfo);
    }

    VKTexture::~VKTexture()
    {
        if (vkImage) {
            device.GetVkDevice().destroyImage(vkImage, nullptr);
        }
        FreeMemory();
    }

    void VKTexture::Destroy()
    {
        delete this;
    }

    vk::Image VKTexture::GetImage() const
    {
        return vkImage;
    }

    void VKTexture::CreateImage(const TextureCreateInfo* createInfo)
    {
        vk::ImageCreateInfo imageInfo = {};
        imageInfo.setArrayLayers(1)
            .setMipLevels(createInfo->mipLevels)
            .setExtent(FromRHI(createInfo->extent))
            .setSamples(static_cast<vk::SampleCountFlagBits>(createInfo->samples))
            .setImageType(VKEnumCast<TextureDimension, vk::ImageType>(createInfo->dimension))
            .setFormat(VKEnumCast<PixelFormat, vk::Format>(createInfo->format))
            .setUsage(GetVkResourceStates(createInfo->usages));

        if (device.GetVkDevice().createImage(&imageInfo, nullptr, &vkImage) != vk::Result::eSuccess) {
            throw VKException("failed to create image");
        }

    }

    void VKTexture::AllocateMemory(const TextureCreateInfo* createInfo)
    {
        vk::MemoryRequirements memoryRequirements = {};
        device.GetVkDevice().getImageMemoryRequirements(vkImage, &memoryRequirements);

        vk::MemoryAllocateInfo memoryInfo = {};
        memoryInfo.setAllocationSize(memoryRequirements.size)
            .setMemoryTypeIndex(device.GetGpu()->FindMemoryType(memoryRequirements.memoryTypeBits,
                                                                vk::MemoryPropertyFlagBits::eDeviceLocal));
        if (device.GetVkDevice().allocateMemory(&memoryInfo, nullptr, &vkDeviceMemory) != vk::Result::eSuccess) {
            throw VKException("failed to allocate memory");
        }

        device.GetVkDevice().bindImageMemory(vkImage, vkDeviceMemory, 0);
    }

    void VKTexture::FreeMemory()
    {
        if (vkDeviceMemory) {
            device.GetVkDevice().free(vkDeviceMemory);
            vkDeviceMemory = nullptr;
        }
    }

}