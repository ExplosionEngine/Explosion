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
            device.GetVkDevice().destroyFence(fence);
        }
    }

    FenceStatus VKFence::GetStatus()
    {
        return signaled ? FenceStatus::signaled : FenceStatus::notReady;
    }

    void VKFence::Reset()
    {
        Assert(device.GetVkDevice().resetFences(1, &fence) == vk::Result::eSuccess);
        signaled = false;
    }

    void VKFence::Wait()
    {
        if (signaled) {
            return;
        }
        Assert(device.GetVkDevice().waitForFences(1, &fence, VK_TRUE, UINT64_MAX) == vk::Result::eSuccess);
    }

    vk::Fence VKFence::GetVkFence()
    {
        return fence;
    }

    void VKFence::CreateVKFence()
    {
        vk::FenceCreateInfo fenceInfo{};

        Assert(device.GetVkDevice().createFence(&fenceInfo, nullptr, &fence) == vk::Result::eSuccess);
    }

    void VKFence::Destroy()
    {
        delete this;
    }

}
