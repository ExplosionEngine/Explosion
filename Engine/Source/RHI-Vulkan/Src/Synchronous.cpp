//
// Created by swtpotato on 2022/8/2.
//

#include <Common/Debug.h>
#include <RHI/Vulkan/Synchronous.h>
#include <RHI/Vulkan/Device.h>

namespace RHI::Vulkan {
    VKFence::VKFence(VKDevice& device_) : Fence(device_), device(device_)
    {
        CreateVKFence();
    }

    VKFence::~VKFence()
    {
        if (fence) {
            vkDestroyFence(device.GetVkDevice(), fence, nullptr);
        }
    }

    void VKFence::Signal(uint32_t value)
    {
        // TODO
    }

    void VKFence::Wait(uint32_t value)
    {
        Assert(vkWaitForFences(device.GetVkDevice(), 1, &fence, VK_TRUE, UINT64_MAX) == VK_SUCCESS);
    }

    VkFence VKFence::GetVkFence()
    {
        return fence;
    }

    void VKFence::CreateVKFence()
    {
        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

        Assert(vkCreateFence(device.GetVkDevice(), &fenceInfo, nullptr, &fence) == VK_SUCCESS);
    }

    void VKFence::Destroy()
    {
        delete this;
    }

}
