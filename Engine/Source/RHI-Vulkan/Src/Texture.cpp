//
// Created by Zach Lee on 2022/3/7.
//

#include <RHI/Vulkan/Texture.h>
#include <RHI/Vulkan/TextureView.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Common.h>
#include <RHI/Vulkan/Queue.h>
#include <RHI/Vulkan/CommandBuffer.h>
#include <RHI/Vulkan/CommandRecorder.h>
#include <RHI/Vulkan/Synchronous.h>

namespace RHI::Vulkan {
    VulkanTexture::VulkanTexture(VulkanDevice& inDevice, const TextureCreateInfo& inCreateInfo, VkImage inNativeImage)
        : Texture(inCreateInfo)
        , device(inDevice)
        , nativeImage(inNativeImage)
        , nativeAspect(VK_IMAGE_ASPECT_COLOR_BIT)
        , ownMemory(false)
    {
        TransitionToInitState(inCreateInfo);
    }

    VulkanTexture::VulkanTexture(VulkanDevice& inDevice, const TextureCreateInfo& inCreateInfo)
        : Texture(inCreateInfo)
        , device(inDevice)
        , nativeImage(VK_NULL_HANDLE)
        , nativeAspect(VK_IMAGE_ASPECT_COLOR_BIT)
        , ownMemory(true)
    {
        CreateNativeImage(inCreateInfo);
        TransitionToInitState(inCreateInfo);
    }

    VulkanTexture::~VulkanTexture()
    {
        if (nativeImage != VK_NULL_HANDLE && ownMemory) {
            vmaDestroyImage(device.GetNativeAllocator(), nativeImage, nativeAllocation);
        }
    }

    Common::UniquePtr<TextureView> VulkanTexture::CreateTextureView(const TextureViewCreateInfo& inCreateInfo)
    {
        return Common::UniquePtr<TextureView>(new VulkanTextureView(*this, device, inCreateInfo));
    }

    VkImage VulkanTexture::GetNative() const
    {
        return nativeImage;
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

    VkImageSubresourceRange VulkanTexture::GetNativeSubResourceFullRange() const
    {
        if (createInfo.dimension == TextureDimension::t3D) {
            return { nativeAspect, 0, createInfo.mipLevels, 0, 1 };
        } else {
            return { nativeAspect, 0, createInfo.mipLevels, 0, createInfo.depthOrArraySize };
        }
    }

    void VulkanTexture::CreateNativeImage(const TextureCreateInfo& inCreateInfo)
    {
        GetAspect(inCreateInfo);

        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.mipLevels = inCreateInfo.mipLevels;
        if (inCreateInfo.dimension == TextureDimension::t3D) {
            imageInfo.extent = { inCreateInfo.width, inCreateInfo.height, inCreateInfo.depthOrArraySize };
            imageInfo.arrayLayers = 1;
        } else {
            imageInfo.extent = { inCreateInfo.width, inCreateInfo.height, 1 };
            imageInfo.arrayLayers = inCreateInfo.depthOrArraySize;
        }
        imageInfo.samples = static_cast<VkSampleCountFlagBits>(inCreateInfo.samples);
        imageInfo.imageType = EnumCast<TextureDimension, VkImageType>(inCreateInfo.dimension);
        imageInfo.format = EnumCast<PixelFormat, VkFormat>(inCreateInfo.format);
        imageInfo.usage = FlagsCast<TextureUsageFlags, VkImageUsageFlags>(inCreateInfo.usages);

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO;

        Assert(vmaCreateImage(device.GetNativeAllocator(), &imageInfo, &allocInfo, &nativeImage, &nativeAllocation, nullptr) == VK_SUCCESS);

#if BUILD_CONFIG_DEBUG
        if (!inCreateInfo.debugName.empty()) {
            device.SetObjectName(VK_OBJECT_TYPE_IMAGE, reinterpret_cast<uint64_t>(nativeImage), inCreateInfo.debugName.c_str());
        }
#endif
    }

    void VulkanTexture::TransitionToInitState(const TextureCreateInfo& inCreateInfo)
    {
        if (inCreateInfo.initialState > TextureState::undefined) {
            Queue* queue = device.GetQueue(QueueType::graphics, 0);
            Assert(queue);

            const auto fence = device.CreateFence(false);
            const auto commandBuffer = device.CreateCommandBuffer();
            const auto commandRecorder = commandBuffer->Begin();
            commandRecorder->ResourceBarrier(Barrier::Transition(this, TextureState::undefined, inCreateInfo.initialState));
            commandRecorder->End();

            QueueSubmitInfo submitInfo {};
            submitInfo.signalFence = fence.Get();
            queue->Submit(commandBuffer.Get(), submitInfo);
            fence->Wait();
        }
    }
}