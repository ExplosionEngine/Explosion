//
// Created by swtpotato on 2022/8/2.
//

#include <Common/Debug.h>
#include <RHI/Vulkan/Synchronous.h>
#include <RHI/Vulkan/Device.h>

namespace RHI::Vulkan {
    VKFence::VKFence(VKDevice& device_) : Fence(device_), device(device_), signaled(false)
    {
        CreateVKFence();
    }

    VKFence::~VKFence()
    {
        if (fence) {
            vkDestroyFence(device.GetVkDevice(), fence, nullptr);
        }
    }

    FenceStatus VKFence::GetStatus()
    {
        return signaled ? FenceStatus::signaled : FenceStatus::notReady;
    }

    void VKFence::Reset()
    {
        Assert(vkResetFences(device.GetVkDevice(), 1, &fence) == VK_SUCCESS);
        signaled = false;
    }

    void VKFence::Wait()
    {
        if (signaled) {
            return;
        }
        Assert(vkWaitForFences(device.GetVkDevice(), 1, &fence, VK_TRUE, UINT64_MAX) == VK_SUCCESS);
    }

    VkFence VKFence::GetVkFence()
    {
        return fence;
    }

    void VKFence::CreateVKFence()
    {
        VkFenceCreateInfo fenceInfo{};

        Assert(vkCreateFence(device.GetVkDevice(), &fenceInfo, nullptr, &fence) == VK_SUCCESS);
    }

    void VKFence::Destroy()
    {
        delete this;
    }

}
