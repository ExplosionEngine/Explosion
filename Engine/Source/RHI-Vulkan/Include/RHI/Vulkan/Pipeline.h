//
// Created by Zach Lee on 2022/4/2.
//

#pragma once

#include <vulkan/vulkan.h>

#include <RHI/Pipeline.h>

namespace RHI::Vulkan {
    class VulkanDevice;
    class VulkanPipelineLayout;

    class VulkanGraphicsPipeline : public GraphicsPipeline {
    public:
        NonCopyable(VulkanGraphicsPipeline)
        VulkanGraphicsPipeline(VulkanDevice& inDevice, const GraphicsPipelineCreateInfo& inCreateInfo);
        ~VulkanGraphicsPipeline() override;
        void Destroy() override;

        VulkanPipelineLayout* GetPipelineLayout() const;
        VkPipeline GetNative();

    private:
        void SavePipelineLayout(const GraphicsPipelineCreateInfo& inCreateInfo);
        void CreateNativeGraphicsPipeline(const GraphicsPipelineCreateInfo& inCreateInfo);

        VulkanDevice& device;
        VulkanPipelineLayout* pipelineLayout;
        VkPipeline nativePipeline = VK_NULL_HANDLE;
    };

}