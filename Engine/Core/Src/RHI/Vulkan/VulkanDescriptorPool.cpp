//
// Created by John Kindem on 2021/5/26.
//

#include <Engine/RHI/Vulkan/VulkanDescriptorPool.h>
#include <Engine/RHI/Vulkan/VulkanDriver.h>
#include <Engine/RHI/Vulkan/VulkanDevice.h>
#include <Engine/RHI/Vulkan/VulkanAdapater.h>

namespace Explosion::RHI {
    VulkanDescriptorPool::VulkanDescriptorPool(VulkanDriver& driver, DescriptorPool::Config config)
        : DescriptorPool(std::move(config)), driver(driver), device(*driver.GetDevice())
    {
        CreateDescriptorPool();
    }

    VulkanDescriptorPool::~VulkanDescriptorPool()
    {
        DestroyDescriptorPool();
    }

    const VkDescriptorPool& VulkanDescriptorPool::GetVkDescriptorPool()
    {
        return vkDescriptorPool;
    }

    void VulkanDescriptorPool::CreateDescriptorPool()
    {
        std::vector<VkDescriptorPoolSize> poolSizes(config.poolSizes.size());
        for (uint32_t i = 0; i < poolSizes.size(); i++) {
            poolSizes[i].type = VkConvert<DescriptorType, VkDescriptorType>(config.poolSizes[i].type);
            poolSizes[i].descriptorCount = config.poolSizes[i].count;
        }

        VkDescriptorPoolCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        createInfo.flags = 0;
        createInfo.maxSets = config.maxSets;
        createInfo.poolSizeCount = poolSizes.size();
        createInfo.pPoolSizes = poolSizes.data();
        createInfo.pNext = nullptr;

        if (vkCreateDescriptorPool(device.GetVkDevice(), &createInfo, nullptr, &vkDescriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool");
        }
    }

    void VulkanDescriptorPool::DestroyDescriptorPool()
    {
        vkDestroyDescriptorPool(device.GetVkDevice(), vkDescriptorPool, nullptr);
    }
}
