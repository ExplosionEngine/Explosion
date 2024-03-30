//
// Created by swtpotato on 2022/8/2.
//

#include <Common/Debug.h>
#include <RHI/Vulkan/Synchronous.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Gpu.h>
#include <RHI/Vulkan/Instance.h>

namespace RHI::Vulkan {
    VulkanFence::VulkanFence(VulkanDevice& inDevice, bool inInitAsSignaled)
        : Fence(inDevice, inInitAsSignaled)
        , device(inDevice)
        , nativeFence(VK_NULL_HANDLE)
    {
        VkFenceCreateInfo fenceCreateInfo {};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = inInitAsSignaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

        Assert(vkCreateFence(device.GetNative(), &fenceCreateInfo, nullptr, &nativeFence) == VK_SUCCESS);
    }

    VulkanFence::~VulkanFence()
    {
        vkDestroyFence(device.GetNative(), nativeFence, nullptr);
    }

    bool VulkanFence::IsSignaled()
    {
        return vkGetFenceStatus(device.GetNative(), nativeFence) == VK_SUCCESS;
    }

    void VulkanFence::Reset()
    {
        std::vector<VkFence> fences = {nativeFence };
        Assert(vkResetFences(device.GetNative(), fences.size(), fences.data()) == VK_SUCCESS);
    }

    void VulkanFence::Wait()
    {
        std::vector<VkFence> fences = {nativeFence };
        Assert(vkWaitForFences(device.GetNative(), fences.size(), fences.data(), VK_TRUE, UINT64_MAX) == VK_SUCCESS);
    }

    void VulkanFence::Destroy()
    {
        delete this;
    }

    VkFence VulkanFence::GetNative() const
    {
        return nativeFence;
    }

    VulkanSemaphore::VulkanSemaphore(VulkanDevice& inDevice)
        : Semaphore(inDevice)
        , device(inDevice)
        , nativeSemaphore(VK_NULL_HANDLE)
    {
        VkSemaphoreCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        Assert(vkCreateSemaphore(device.GetNative(), &createInfo, nullptr, &nativeSemaphore) == VK_SUCCESS);
    }

    VulkanSemaphore::~VulkanSemaphore()
    {
        vkDestroySemaphore(device.GetNative(), nativeSemaphore, nullptr);
    }

    void VulkanSemaphore::Destroy()
    {
        delete this;
    }

    VkSemaphore VulkanSemaphore::GetNative() const
    {
        return nativeSemaphore;
    }
}
