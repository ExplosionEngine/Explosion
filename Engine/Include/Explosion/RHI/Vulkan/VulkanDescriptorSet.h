//
// Created by John Kindem on 2021/5/26.
//

#ifndef EXPLOSION_VULKANDESCRIPTOR_H
#define EXPLOSION_VULKANDESCRIPTOR_H

#include <vector>

#include <vulkan/vulkan.h>

#include <Explosion/RHI/Common/DescriptorSet.h>

namespace Explosion::RHI {
    class VulkanDriver;
    class VulkanDevice;
    class VulkanDescriptorPool;
    class VulkanGraphicsPipeline;

    class VulkanDescriptorSet : public DescriptorSet {
    public:
        VulkanDescriptorSet(VulkanDriver& driver, VulkanDescriptorPool* descriptorPool, VulkanGraphicsPipeline* pipeline);
        ~VulkanDescriptorSet() override;
        void WriteDescriptors(const std::vector<DescriptorWriteInfo>& writeInfos) override;

        const VkDescriptorSet& GetVkDescriptorSet();

    private:
        void AllocateDescriptorSet();
        void FreeDescriptorSet();

        VulkanDriver& driver;
        VulkanDevice& device;
        VulkanDescriptorPool* descriptorPool;
        VulkanGraphicsPipeline* pipeline;
        VkDescriptorSet vkDescriptorSet = VK_NULL_HANDLE;
    };
}

#endif //EXPLOSION_VULKANDESCRIPTOR_H
