//
// Created by John Kindem on 2021/4/30.
//

#include <stdexcept>

#include <Explosion/Driver/Signal.h>
#include <Explosion/Driver/Driver.h>

namespace Explosion {
    Signal::Signal(Driver& driver)
        : GpuRes(driver), device(*driver.GetDevice()) {}

    Signal::~Signal() = default;

    void Signal::OnCreate()
    {
        GpuRes::OnCreate();
        CreateSemaphore();
    }

    void Signal::OnDestroy()
    {
        GpuRes::OnDestroy();
        DestroySemaphore();
    }

    const VkSemaphore& Signal::GetVkSemaphore()
    {
        return vkSemaphore;
    }

    void Signal::CreateSemaphore()
    {
        VkSemaphoreCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;

        if (vkCreateSemaphore(device.GetVkDevice(), &createInfo, nullptr, &vkSemaphore) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vulkan semaphore");
        }
    }

    void Signal::DestroySemaphore()
    {
        vkDestroySemaphore(device.GetVkDevice(), vkSemaphore, nullptr);
    }
}
