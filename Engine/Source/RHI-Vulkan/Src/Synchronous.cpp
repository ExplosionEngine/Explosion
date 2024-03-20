//
// Created by swtpotato on 2022/8/2.
//

#include <Common/Debug.h>
#include <RHI/Vulkan/Synchronous.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Gpu.h>
#include <RHI/Vulkan/Instance.h>

namespace RHI::Vulkan {
    VKFence::VKFence(VKDevice& device_)
        : Fence(device_)
        , device(device_)
        , vkWaitSemaphoresKhrFunc(reinterpret_cast<PFN_vkWaitSemaphoresKHR>(vkGetInstanceProcAddr(device_.GetGpu().GetInstance().GetVkInstance(), "vkWaitSemaphoresKHR")))
        , vkSignalSemaphoreKhrFunc(reinterpret_cast<PFN_vkSignalSemaphoreKHR>(vkGetInstanceProcAddr(device_.GetGpu().GetInstance().GetVkInstance(), "vkSignalSemaphoreKHR")))
    {
        VkSemaphoreTypeCreateInfo typeCreateInfo {};
        typeCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
        typeCreateInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
        typeCreateInfo.initialValue = 0;

        VkSemaphoreCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        createInfo.pNext = &typeCreateInfo;

        Assert(vkCreateSemaphore(device.GetVkDevice(), &createInfo, nullptr, &vkTimelineSemaphore) == VK_SUCCESS);
    }

    VKFence::~VKFence()
    {
        vkDestroySemaphore(device.GetVkDevice(), vkTimelineSemaphore, nullptr);
    }

    void VKFence::Signal(uint32_t value)
    {
        VkSemaphoreSignalInfoKHR signalInfo {};
        signalInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO_KHR;
        signalInfo.semaphore = vkTimelineSemaphore;
        signalInfo.value = value;

        Assert(vkSignalSemaphoreKhrFunc(device.GetVkDevice(), &signalInfo) == VK_SUCCESS);
    }

    void VKFence::Wait(uint32_t value)
    {
        std::vector<uint64_t> values = { value };

        VkSemaphoreWaitInfo waitInfo {};
        waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO_KHR;
        waitInfo.pSemaphores = &vkTimelineSemaphore;
        waitInfo.semaphoreCount = 1;
        waitInfo.pValues = values.data();

        Assert(vkWaitSemaphoresKhrFunc(device.GetVkDevice(), &waitInfo, UINT64_MAX) == VK_SUCCESS);
    }

    void VKFence::Destroy()
    {
        delete this;
    }

    VkSemaphore VKFence::GetTimelineSemaphore() const
    {
        return vkTimelineSemaphore;
    }
}
