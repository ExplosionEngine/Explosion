//
// Created by swtpotato on 2022/8/2.
//

#include <Common/Debug.h>
#include <RHI/Vulkan/Synchronous.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Gpu.h>
#include <RHI/Vulkan/Instance.h>

namespace RHI::Vulkan {
    VKFence::VKFence(VKDevice& inDevice, bool initAsSignaled)
        : Fence(inDevice, initAsSignaled)
        , device(inDevice)
        , vkFence(VK_NULL_HANDLE)
    {
        VkFenceCreateInfo fenceCreateInfo {};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = initAsSignaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

        Assert(vkCreateFence(device.GetVkDevice(), &fenceCreateInfo, nullptr, &vkFence) == VK_SUCCESS);
    }

    VKFence::~VKFence()
    {
        vkDestroyFence(device.GetVkDevice(), vkFence, nullptr);
    }

    bool VKFence::IsSignaled()
    {
        return vkGetFenceStatus(device.GetVkDevice(), vkFence) == VK_SUCCESS;
    }

    void VKFence::Reset()
    {
        std::vector<VkFence> fences = { vkFence };
        Assert(vkResetFences(device.GetVkDevice(), fences.size(), fences.data()) == VK_SUCCESS);
    }

    void VKFence::Wait()
    {
        std::vector<VkFence> fences = { vkFence };
        Assert(vkWaitForFences(device.GetVkDevice(), fences.size(), fences.data(), VK_TRUE, UINT64_MAX) == VK_SUCCESS);
    }

    void VKFence::Destroy()
    {
        delete this;
    }

    VkFence VKFence::GetNative() const
    {
        return vkFence;
    }
}
