//
// Created by John Kindem on 2021/4/26.
//

#ifndef EXPLOSION_VULKANGRAPHICSPIPELINE_H
#define EXPLOSION_VULKANGRAPHICSPIPELINE_H

#include <vector>

#include <vulkan/vulkan.h>

#include <Explosion/RHI/Common/GraphicsPipeline.h>

namespace Explosion::RHI {
    class VulkanDriver;
    class VulkanDevice;
    class VulkanRenderPass;

    class VulkanGraphicsPipeline : public GraphicsPipeline {
    public:
        explicit VulkanGraphicsPipeline(VulkanDriver& driver, Config config);
        ~VulkanGraphicsPipeline() override;
        const VkPipelineLayout& GetVkPipelineLayout();
        const VkPipeline& GetVkPipeline();
        const VkDescriptorSetLayout& GetVkDescriptorSetLayout();

    private:
        VkShaderModule CreateShaderModule(const std::vector<char>& code);
        void DestroyShaderModule(const VkShaderModule& shaderModule);

        void CreateDescriptorPool();
        void DestroyDescriptorPool();

        void CreateDescriptorSetLayout();
        void DestroyDescriptorSetLayout();

        void CreatePipelineLayout();
        void DestroyPipelineLayout();

        void CreateGraphicsPipeline();
        void DestroyGraphicsPipeline();

        VulkanDriver& driver;
        VulkanDevice& device;
        VkDescriptorPool vkDescriptorPool = VK_NULL_HANDLE;
        VkDescriptorSetLayout vkDescriptorSetLayout = VK_NULL_HANDLE;
        VkPipelineLayout vkPipelineLayout = VK_NULL_HANDLE;
        VkPipeline vkPipeline = VK_NULL_HANDLE;
    };
}

#endif //EXPLOSION_VULKANGRAPHICSPIPELINE_H
