//
// Created by Zach Lee on 2022/4/4.
//

#pragma once

#include <vulkan/vulkan.hpp>
#include <RHI/Pipeline.h>
#include <Common/Utility.h>

namespace RHI::Vulkan {
    class VKDevice;

    class VKRenderPass {
    public:
        NON_COPYABLE(VKRenderPass)
        explicit VKRenderPass(VKDevice& device, const GraphicsPipelineCreateInfo* createInfo);
        ~VKRenderPass();

    private:
        void CreateNativeRenderPass(const GraphicsPipelineCreateInfo* createInfo);
        VKDevice& device;
        vk::RenderPass renderPass = VK_NULL_HANDLE;
    };
}