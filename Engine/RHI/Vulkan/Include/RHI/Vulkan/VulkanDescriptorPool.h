//
// Created by John Kindem on 2021/5/26.
//

#ifndef EXPLOSION_VULKANDESCRIPTORPOOL_H
#define EXPLOSION_VULKANDESCRIPTORPOOL_H

#include <vulkan/vulkan.h>

#include <RHI/DescriptorPool.h>
#include <RHI/Vulkan/Api.h>

namespace Explosion::RHI {
    class VulkanDriver;
    class VulkanDevice;

    class VulkanDescriptorPool : public DescriptorPool {
    public:
        VulkanDescriptorPool(VulkanDriver& driver, Config config);
        ~VulkanDescriptorPool() override;
        const VkDescriptorPool& GetVkDescriptorPool();

    private:
        void CreateDescriptorPool();
        void DestroyDescriptorPool();

        VulkanDriver& driver;
        VulkanDevice& device;
        VkDescriptorPool vkDescriptorPool = VK_NULL_HANDLE;
    };
}

#endif //EXPLOSION_VULKANDESCRIPTORPOOL_H
