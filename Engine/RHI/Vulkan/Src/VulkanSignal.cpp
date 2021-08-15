//
// Created by John Kindem on 2021/4/30.
//

#include <stdexcept>

#include <RHI/Vulkan/VulkanSignal.h>
#include <RHI/Vulkan/VulkanDriver.h>

namespace Explosion::RHI {
    VulkanSignal::VulkanSignal(VulkanDriver& driver)
        : Signal(), driver(driver), device(*driver.GetDevice())
    {
        CreateSemaphore();
    }

    VulkanSignal::~VulkanSignal()
    {
        DestroySemaphore();
    }

    const VkSemaphore& VulkanSignal::GetVkSemaphore()
    {
        return vkSemaphore;
    }

    void VulkanSignal::CreateSemaphore()
    {
        VkSemaphoreCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;

        if (vkCreateSemaphore(device.GetVkDevice(), &createInfo, nullptr, &vkSemaphore) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vulkan semaphore");
        }
    }

    void VulkanSignal::DestroySemaphore()
    {
        vkDestroySemaphore(device.GetVkDevice(), vkSemaphore, nullptr);
    }
}
