//
// Created by John Kindem on 2021/5/27.
//

#include <Explosion/RHI/Vulkan/VulkanDescriptorSet.h>
#include <Explosion/RHI/Vulkan/VulkanDriver.h>
#include <Explosion/RHI/Vulkan/VulkanDevice.h>
#include <Explosion/RHI/Vulkan/VulkanDescriptorPool.h>
#include <Explosion/RHI/Vulkan/VulkanGraphicsPipeline.h>

namespace Explosion::RHI {
    VulkanDescriptorSet::VulkanDescriptorSet(VulkanDriver& driver, VulkanDescriptorPool* descriptorPool, VulkanGraphicsPipeline* pipeline)
        : DescriptorSet(), driver(driver), device(*driver.GetDevice()), descriptorPool(descriptorPool), pipeline(pipeline)
    {
        AllocateDescriptorSet();
    }

    VulkanDescriptorSet::~VulkanDescriptorSet()
    {
        FreeDescriptorSet();
    }

    void VulkanDescriptorSet::WriteDescriptors(const std::vector<DescriptorWriteInfo>& writeInfos)
    {
        // TODO
    }

    const VkDescriptorSet& VulkanDescriptorSet::GetVkDescriptorSet()
    {
        return vkDescriptorSet;
    }

    void VulkanDescriptorSet::AllocateDescriptorSet()
    {
        VkDescriptorSetAllocateInfo allocateInfo {};
        allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocateInfo.descriptorPool = descriptorPool->GetVkDescriptorPool();
        allocateInfo.pSetLayouts = &pipeline->GetVkDescriptorSetLayout();
        allocateInfo.descriptorSetCount = 1;
        allocateInfo.pNext = nullptr;

        if (vkAllocateDescriptorSets(device.GetVkDevice(), &allocateInfo, &vkDescriptorSet) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor set");
        }
    }

    void VulkanDescriptorSet::FreeDescriptorSet()
    {
        vkFreeDescriptorSets(device.GetVkDevice(), descriptorPool->GetVkDescriptorPool(), 1, &vkDescriptorSet);
    }
}
