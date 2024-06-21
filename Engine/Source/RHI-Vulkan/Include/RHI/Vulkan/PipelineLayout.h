//
// Created by Zach Lee on 2022/4/2.
//

#pragma once

#include <vulkan/vulkan.h>

#include <RHI/PipelineLayout.h>

namespace RHI::Vulkan {
    class VulkanDevice;

    class VulkanPipelineLayout : public PipelineLayout {
    public:
        NonCopyable(VulkanPipelineLayout)
        VulkanPipelineLayout(VulkanDevice& inDevice, const PipelineLayoutCreateInfo& inCreateInfo);
        ~VulkanPipelineLayout() override;

        VkPipelineLayout GetNative() const;

    private:
        void CreateNativePipelineLayout(const PipelineLayoutCreateInfo& inCreateInfo);

        VulkanDevice& device;
        VkPipelineLayout nativePipelineLayout;
    };
}