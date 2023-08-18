//
// Created by Zach Lee on 2022/4/2.
//

#pragma once
#include <RHI/Pipeline.h>
#include <vulkan/vulkan.h>

namespace RHI::Vulkan {

    class VKDevice;
    class VKPipelineLayout;

    class VKGraphicsPipeline : public GraphicsPipeline {
    public:
        NonCopyable(VKGraphicsPipeline)
        VKGraphicsPipeline(VKDevice& device, const GraphicsPipelineCreateInfo& createInfo);
        ~VKGraphicsPipeline() override;
        void Destroy() override;

        VkPipeline GetVkPipeline();
        VkRenderPass GetVkRenderPass();
        VKPipelineLayout* GetPipelineLayout() const;

    private:
        void SavePipelineLayout(const GraphicsPipelineCreateInfo& createInfo);
        void CreateNativeGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo);
        void CreateNativeRenderPass(const GraphicsPipelineCreateInfo& createInfo);

        VKDevice& device;
        VKPipelineLayout* pipelineLayout;
        VkRenderPass renderPass = VK_NULL_HANDLE;
        VkPipeline pipeline = VK_NULL_HANDLE;
    };

}