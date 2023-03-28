//
// Created by Zach Lee on 2022/4/2.
//

#pragma once
#include <RHI/Pipeline.h>
#include <vulkan/vulkan.hpp>

namespace RHI::Vulkan {

    class VKDevice;
    class VKPipelineLayout;

    class VKGraphicsPipeline : public GraphicsPipeline {
    public:
        NON_COPYABLE(VKGraphicsPipeline)
        VKGraphicsPipeline(VKDevice& device, const GraphicsPipelineCreateInfo& createInfo);
        ~VKGraphicsPipeline() override;
        void Destroy() override;

        vk::Pipeline GetVkPipeline();
        vk::RenderPass GetVkRenderPass();
        VKPipelineLayout* GetPipelineLayout() const;

    private:
        void SavePipelineLayout(const GraphicsPipelineCreateInfo& createInfo);
        void CreateNativeGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo);
        void CreateNativeRenderPass(const GraphicsPipelineCreateInfo& createInfo);

        VKDevice& device;
        VKPipelineLayout* pipelineLayout;
        vk::RenderPass renderPass = VK_NULL_HANDLE;
        vk::Pipeline pipeline = VK_NULL_HANDLE;
    };

}