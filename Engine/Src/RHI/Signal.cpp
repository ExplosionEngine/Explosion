//
// Created by John Kindem on 2021/4/30.
//

#include <stdexcept>

#include <Explosion/RHI/Signal.h>
#include <Explosion/RHI/Driver.h>

namespace Explosion::RHI {
    Signal::Signal(Driver& driver)
        : driver(driver), device(*driver.GetDevice())
    {
        CreateSemaphore();
    }

    Signal::~Signal()
    {
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
