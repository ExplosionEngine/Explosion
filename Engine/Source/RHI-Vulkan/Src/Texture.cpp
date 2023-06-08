//
// Created by Zach Lee on 2022/3/7.
//

#include <RHI/Vulkan/Texture.h>
#include <RHI/Vulkan/TextureView.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Common.h>
#include <RHI/Vulkan/Gpu.h>
#include <RHI/Vulkan/Queue.h>
#include <RHI/Vulkan/CommandBuffer.h>
#include <RHI/Vulkan/CommandEncoder.h>
#include <RHI/Vulkan/Synchronous.h>

namespace RHI::Vulkan {
    static VkImageUsageFlags GetVkResourceStates(TextureUsageFlags textureUsages)
    {
        static std::unordered_map<TextureUsageBits, VkImageUsageFlags> rules = {
            { TextureUsageBits::copySrc,          VK_IMAGE_USAGE_TRANSFER_SRC_BIT },
            { TextureUsageBits::copyDst,          VK_IMAGE_USAGE_TRANSFER_DST_BIT },
            { TextureUsageBits::textureBinding,   VK_IMAGE_USAGE_SAMPLED_BIT },
            { TextureUsageBits::storageBinding,   VK_IMAGE_USAGE_STORAGE_BIT },
            { TextureUsageBits::renderAttachment, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT },
            { TextureUsageBits::depthStencilAttachment, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT },
        };

        VkImageUsageFlags result = {};
        for (const auto& rule : rules) {
            if (textureUsages & rule.first) {
                result |= rule.second;
            }
        }
        return result;
    }

    VKTexture::VKTexture(VKDevice& dev, const TextureCreateInfo& createInfo, VkImage image)
        : Texture(createInfo), device(dev), vkDeviceMemory(VK_NULL_HANDLE), vkImage(image), ownMemory(false), extent(createInfo.extent), format(createInfo.format), mipLevels(createInfo.mipLevels), samples(createInfo.samples)
    {
    }

    VKTexture::VKTexture(VKDevice& dev, const TextureCreateInfo& createInfo)
        : Texture(createInfo), device(dev), vkImage(VK_NULL_HANDLE), ownMemory(true), extent(createInfo.extent), format(createInfo.format), mipLevels(createInfo.mipLevels), samples(createInfo.samples)
    {
        CreateImage(createInfo);
        AllocateMemory(createInfo);
        TransitionToInitState(createInfo);
    }

    VKTexture::~VKTexture()
    {
        if (vkImage && ownMemory) {
            vkDestroyImage(device.GetVkDevice(), vkImage, nullptr);
        }

        if (vkImageView) {
            vkDestroyImageView(device.GetVkDevice(), vkImageView, nullptr);
        }

        FreeMemory();
    }

    void VKTexture::Destroy()
    {
        delete this;
    }

    TextureView* VKTexture::CreateTextureView(const TextureViewCreateInfo& createInfo)
    {
        return new VKTextureView(*this, device, createInfo);
    }

    VkImage VKTexture::GetImage() const
    {
        return vkImage;
    }

    Extent<3> VKTexture::GetExtent() const
    {
        return extent;
    }

    void VKTexture::GetAspect(const RHI::TextureCreateInfo& createInfo)
    {
        if (createInfo.usages & TextureUsageBits::depthStencilAttachment) {
            aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
            if (createInfo.format == PixelFormat::d32FloatS8Uint || createInfo.format == PixelFormat::d24UnormS8Uint) {
                aspect |= VK_IMAGE_ASPECT_STENCIL_BIT;
            }
        }
    }

    VkImageSubresourceRange VKTexture::GetFullRange()
    {
        return { aspect, 0, mipLevels, 0, extent.z };
    }

    void VKTexture::CreateImage(const TextureCreateInfo& createInfo)
    {
        GetAspect(createInfo);

        VkImageCreateInfo imageInfo = {};
        imageInfo.arrayLayers = extent.z;
        imageInfo.mipLevels = mipLevels;
        imageInfo.extent = FromRHI(createInfo.extent);
        imageInfo.samples = static_cast<VkSampleCountFlagBits>(createInfo.samples);
        imageInfo.imageType = VKEnumCast<TextureDimension, VkImageType>(createInfo.dimension);
        imageInfo.format = VKEnumCast<PixelFormat, VkFormat>(createInfo.format);
        imageInfo.usage = GetVkResourceStates(createInfo.usages);

        Assert(vkCreateImage(device.GetVkDevice(), &imageInfo, nullptr, &vkImage) == VK_SUCCESS);
    }

    void VKTexture::AllocateMemory(const TextureCreateInfo& createInfo)
    {
        VkMemoryRequirements memoryRequirements = {};
        vkGetImageMemoryRequirements(device.GetVkDevice(), vkImage, &memoryRequirements);

        VkMemoryAllocateInfo memoryInfo = {};
        memoryInfo.allocationSize = memoryRequirements.size;
        memoryInfo.memoryTypeIndex = device.GetGpu().FindMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        Assert(vkAllocateMemory(device.GetVkDevice(), &memoryInfo, nullptr, &vkDeviceMemory) == VK_SUCCESS);
        vkBindImageMemory(device.GetVkDevice(), vkImage, vkDeviceMemory, 0);
    }

    void VKTexture::FreeMemory()
    {
        if (vkDeviceMemory) {
            vkFreeMemory(device.GetVkDevice(), vkDeviceMemory, nullptr);
            vkDeviceMemory = nullptr;
        }
    }

    PixelFormat VKTexture::GetFormat() const
    {
        return format;
    }

    void VKTexture::TransitionToInitState(const TextureCreateInfo& createInfo)
    {
        if (createInfo.initialState > TextureState::undefined) {
            Queue* queue = device.GetQueue(QueueType::graphics, 0);
            Assert(queue);

            Common::UniqueRef<Fence> fence = device.CreateFence();
            Common::UniqueRef<CommandBuffer> commandBuffer = device.CreateCommandBuffer();
            Common::UniqueRef<CommandEncoder> commandEncoder = commandBuffer->Begin();
            commandEncoder->ResourceBarrier(Barrier::Transition(this, TextureState::undefined, createInfo.initialState));
            commandEncoder->End();

            queue->Submit(commandBuffer.Get(), fence.Get());
            fence->Wait();
        }
    }
}