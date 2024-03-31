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

    VulkanTexture::VulkanTexture(VulkanDevice& inDevice, const TextureCreateInfo& inCreateInfo, VkImage inNativeImage)
        : Texture(inCreateInfo), device(inDevice), nativeImage(inNativeImage), ownMemory(false), extent(inCreateInfo.extent), format(inCreateInfo.format), mipLevels(inCreateInfo.mipLevels), samples(inCreateInfo.samples)
    {
    }

    VulkanTexture::VulkanTexture(VulkanDevice& inDevice, const TextureCreateInfo& inCreateInfo)
        : Texture(inCreateInfo), device(inDevice), nativeImage(VK_NULL_HANDLE), ownMemory(true), extent(inCreateInfo.extent), format(inCreateInfo.format), mipLevels(inCreateInfo.mipLevels), samples(inCreateInfo.samples)
    {
        CreateNativeImage(inCreateInfo);
        TransitionToInitState(inCreateInfo);
    }

    VulkanTexture::~VulkanTexture()
    {
        if (nativeImage && ownMemory) {
            vmaDestroyImage(device.GetNativeAllocator(), nativeImage, nativeAllocation);
        }

        if (nativeImageView) {
            vkDestroyImageView(device.GetNative(), nativeImageView, nullptr);
        }
    }

    void VulkanTexture::Destroy()
    {
        delete this;
    }

    Holder<TextureView> VulkanTexture::CreateTextureView(const TextureViewCreateInfo& inCreateInfo)
    {
        return Common::UniqueRef<TextureView>(new VulkanTextureView(*this, device, inCreateInfo));
    }

    VkImage VulkanTexture::GetNative() const
    {
        return nativeImage;
    }

    Common::UVec3 VulkanTexture::GetExtent() const
    {
        return extent;
    }

    void VulkanTexture::GetAspect(const RHI::TextureCreateInfo& inCreateInfo)
    {
        if (inCreateInfo.usages & TextureUsageBits::depthStencilAttachment) {
            nativeAspect = VK_IMAGE_ASPECT_DEPTH_BIT;
            if (inCreateInfo.format == PixelFormat::d32FloatS8Uint || inCreateInfo.format == PixelFormat::d24UnormS8Uint) {
                nativeAspect |= VK_IMAGE_ASPECT_STENCIL_BIT;
            }
        }
    }

    VkImageSubresourceRange VulkanTexture::GetNativeSubResourceFullRange()
    {
        return {nativeAspect, 0, mipLevels, 0, extent.z };
    }

    void VulkanTexture::CreateNativeImage(const TextureCreateInfo& inCreateInfo)
    {
        GetAspect(inCreateInfo);

        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.arrayLayers = extent.z;
        imageInfo.mipLevels = mipLevels;
        imageInfo.extent = FromRHI(inCreateInfo.extent);
        imageInfo.samples = static_cast<VkSampleCountFlagBits>(inCreateInfo.samples);
        imageInfo.imageType = VKEnumCast<TextureDimension, VkImageType>(inCreateInfo.dimension);
        imageInfo.format = VKEnumCast<PixelFormat, VkFormat>(inCreateInfo.format);
        imageInfo.usage = GetVkResourceStates(inCreateInfo.usages);

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO;

        Assert(vmaCreateImage(device.GetNativeAllocator(), &imageInfo, &allocInfo, &nativeImage, &nativeAllocation, nullptr) == VK_SUCCESS);

#if BUILD_CONFIG_DEBUG
        if (!inCreateInfo.debugName.empty()) {
            device.SetObjectName(VK_OBJECT_TYPE_IMAGE, reinterpret_cast<uint64_t>(nativeImage), inCreateInfo.debugName.c_str());
        }
#endif
    }

    PixelFormat VulkanTexture::GetFormat() const
    {
        return format;
    }

    void VulkanTexture::TransitionToInitState(const TextureCreateInfo& inCreateInfo)
    {
        if (inCreateInfo.initialState > TextureState::undefined) {
            Queue* queue = device.GetQueue(QueueType::graphics, 0);
            Assert(queue);

            Common::UniqueRef<Fence> fence = device.CreateFence(false);
            Common::UniqueRef<CommandBuffer> commandBuffer = device.CreateCommandBuffer();
            Common::UniqueRef<CommandEncoder> commandEncoder = commandBuffer->Begin();
            commandEncoder->ResourceBarrier(Barrier::Transition(this, TextureState::undefined, inCreateInfo.initialState));
            commandEncoder->End();

            QueueSubmitInfo submitInfo {};
            submitInfo.signalFence = fence.Get();
            queue->Submit(commandBuffer.Get(), submitInfo);
            fence->Wait();
        }
    }
}