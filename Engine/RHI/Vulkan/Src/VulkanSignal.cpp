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
        CreateSignal();
    }

    VulkanSignal::~VulkanSignal()
    {
        DestroySignal();
    }

    const VkSemaphore& VulkanSignal::GetVkSemaphore()
    {
        return vkSemaphore;
    }

    void VulkanSignal::CreateSignal()
    {
        VkSemaphoreCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;

        if (vkCreateSemaphore(device.GetVkDevice(), &createInfo, nullptr, &vkSemaphore) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vulkan semaphore");
        }
    }

    void VulkanSignal::DestroySignal()
    {
        vkDestroySemaphore(device.GetVkDevice(), vkSemaphore, nullptr);
    }
}
