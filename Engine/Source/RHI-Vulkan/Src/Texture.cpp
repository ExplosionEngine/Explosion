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
    static vk::ImageUsageFlags GetVkResourceStates(TextureUsageFlags textureUsages)
    {
        static std::unordered_map<TextureUsageBits, vk::ImageUsageFlags> rules = {
            { TextureUsageBits::copySrc,          vk::ImageUsageFlagBits::eTransferSrc },
            { TextureUsageBits::copyDst,          vk::ImageUsageFlagBits::eTransferDst },
            { TextureUsageBits::textureBinding,   vk::ImageUsageFlagBits::eSampled },
            { TextureUsageBits::storageBinding,   vk::ImageUsageFlagBits::eStorage },
            { TextureUsageBits::renderAttachment, vk::ImageUsageFlagBits::eColorAttachment },
            { TextureUsageBits::depthStencilAttachment, vk::ImageUsageFlagBits::eDepthStencilAttachment },
        };

        vk::ImageUsageFlags result = {};
        for (const auto& rule : rules) {
            if (textureUsages & rule.first) {
                result |= rule.second;
            }
        }
        return result;
    }

    VKTexture::VKTexture(VKDevice& dev, const TextureCreateInfo& createInfo, vk::Image image)
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
            device.GetVkDevice().destroyImage(vkImage, nullptr);
        }

        if (vkImageView) {
            device.GetVkDevice().destroyImageView(vkImageView, nullptr);
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

    vk::Image VKTexture::GetImage() const
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
            aspect = vk::ImageAspectFlagBits::eDepth;
            if (createInfo.format == PixelFormat::d32FloatS8Uint || createInfo.format == PixelFormat::d24UnormS8Uint) {
                aspect |= vk::ImageAspectFlagBits::eStencil;
            }
        }
    }

    vk::ImageSubresourceRange VKTexture::GetFullRange()
    {
        return { aspect, 0, mipLevels, 0, extent.z };
    }

    void VKTexture::CreateImage(const TextureCreateInfo& createInfo)
    {
        GetAspect(createInfo);

        vk::ImageCreateInfo imageInfo = {};
        imageInfo.setArrayLayers(1)
            .setMipLevels(createInfo.mipLevels)
            .setExtent(FromRHI(createInfo.extent))
            .setSamples(static_cast<vk::SampleCountFlagBits>(createInfo.samples))
            .setImageType(VKEnumCast<TextureDimension, vk::ImageType>(createInfo.dimension))
            .setFormat(VKEnumCast<PixelFormat, vk::Format>(createInfo.format))
            .setUsage(GetVkResourceStates(createInfo.usages));

        Assert(device.GetVkDevice().createImage(&imageInfo, nullptr, &vkImage) == vk::Result::eSuccess);
    }

    void VKTexture::AllocateMemory(const TextureCreateInfo& createInfo)
    {
        vk::MemoryRequirements memoryRequirements = {};
        device.GetVkDevice().getImageMemoryRequirements(vkImage, &memoryRequirements);

        vk::MemoryAllocateInfo memoryInfo = {};
        memoryInfo.setAllocationSize(memoryRequirements.size)
            .setMemoryTypeIndex(device.GetGpu().FindMemoryType(memoryRequirements.memoryTypeBits,
                                                                vk::MemoryPropertyFlagBits::eDeviceLocal));
        Assert(device.GetVkDevice().allocateMemory(&memoryInfo, nullptr, &vkDeviceMemory) == vk::Result::eSuccess);
        device.GetVkDevice().bindImageMemory(vkImage, vkDeviceMemory, 0);
    }

    void VKTexture::FreeMemory()
    {
        if (vkDeviceMemory) {
            device.GetVkDevice().free(vkDeviceMemory);
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