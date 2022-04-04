//
// Created by Zach Lee on 2022/4/2.
//

#pragma once
#include <RHI/Pipeline.h>
#include <vulkan/vulkan.hpp>

namespace RHI::Vulkan {

    class VKDevice;

    class VKGraphicsPipeline : public GraphicsPipeline {
    public:
        NON_COPYABLE(VKGraphicsPipeline)
        VKGraphicsPipeline(VKDevice& device, const GraphicsPipelineCreateInfo* createInfo);
        ~VKGraphicsPipeline() override;

        void Destroy() override;
    private:
        void CreateNativeGraphicsPipeline(const GraphicsPipelineCreateInfo* createInfo);

        VKDevice& device;
        vk::Pipeline pipeline = VK_NULL_HANDLE;
    };

}