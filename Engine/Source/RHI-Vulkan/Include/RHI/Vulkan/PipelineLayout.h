//
// Created by Zach Lee on 2022/4/2.
//

#pragma once
#include <RHI/PipelineLayout.h>
#include <vulkan/vulkan.h>

namespace RHI::Vulkan {
    class VKDevice;

    class VKPipelineLayout : public PipelineLayout {
    public:
        NON_COPYABLE(VKPipelineLayout)
        VKPipelineLayout(VKDevice& device, const PipelineLayoutCreateInfo& createInfo);
        ~VKPipelineLayout() override;

        void Destroy() override;

        VkPipelineLayout GetVkPipelineLayout() const;
    private:
        void CreateNativePipelineLayout(const PipelineLayoutCreateInfo& createInfo);

        VKDevice& device;
        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    };


}