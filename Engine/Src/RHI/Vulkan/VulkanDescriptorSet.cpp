//
// Created by John Kindem on 2021/5/27.
//

#include <stdexcept>

#include <Explosion/RHI/Vulkan/VulkanDescriptorSet.h>
#include <Explosion/RHI/Vulkan/VulkanDriver.h>
#include <Explosion/RHI/Vulkan/VulkanDevice.h>
#include <Explosion/RHI/Vulkan/VulkanDescriptorPool.h>
#include <Explosion/RHI/Vulkan/VulkanGraphicsPipeline.h>
#include <Explosion/RHI/Vulkan/VulkanAdapater.h>
#include <Explosion/RHI/Vulkan/VulkanBuffer.h>
#include <Explosion/RHI/Vulkan/VulkanImageView.h>
#include <Explosion/RHI/Vulkan/VulkanSampler.h>

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
        for (auto& writeInfo : writeInfos) {
            VkDescriptorBufferInfo bufferInfo {};
            if (writeInfo.bufferInfo != nullptr) {
                bufferInfo.buffer = dynamic_cast<VulkanBuffer*>(writeInfo.bufferInfo->buffer)->GetVkBuffer();
                bufferInfo.offset = writeInfo.bufferInfo->offset;
                bufferInfo.range = writeInfo.bufferInfo->range;
            }

            VkDescriptorImageInfo imageInfo {};
            if (writeInfo.textureInfo != nullptr) {
                imageInfo.sampler = dynamic_cast<VulkanSampler*>(writeInfo.textureInfo->sampler)->GetVkSampler();
                imageInfo.imageView = dynamic_cast<VulkanImageView*>(writeInfo.textureInfo->imageView)->GetVkImageView();
                imageInfo.imageLayout = VkConvert<ImageLayout, VkImageLayout>(writeInfo.textureInfo->imageLayout);
            }

            VkWriteDescriptorSet writeDescriptorSet {};
            writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptorSet.dstSet = vkDescriptorSet;
            writeDescriptorSet.dstBinding = writeInfo.binding;
            writeDescriptorSet.dstArrayElement = 0;
            writeDescriptorSet.descriptorType = VkConvert<DescriptorType, VkDescriptorType>(writeInfo.type);
            writeDescriptorSet.descriptorCount = 1;
            writeDescriptorSet.pBufferInfo = writeInfo.bufferInfo == nullptr ? nullptr : &bufferInfo;
            writeDescriptorSet.pImageInfo = writeInfo.bufferInfo == nullptr ? nullptr : &imageInfo;

            vkUpdateDescriptorSets(device.GetVkDevice(), 1, &writeDescriptorSet, 0, nullptr);
        }
    }

    const VkDescriptorSet& VulkanDescriptorSet::GetVkDescriptorSet() const
    {
        return vkDescriptorSet;
    }

    const VkPipelineLayout& VulkanDescriptorSet::GetPipelineLayout() const
    {
        return pipeline->GetVkPipelineLayout();
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
