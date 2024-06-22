//
// Created by Zach Lee on 2022/4/2.
//

#pragma once

#include <vulkan/vulkan.h>

#include <RHI/Pipeline.h>

namespace RHI::Vulkan {
    class VulkanDevice;
    class VulkanPipelineLayout;

    class VulkanRasterPipeline final : public RasterPipeline {
    public:
        NonCopyable(VulkanRasterPipeline)
        VulkanRasterPipeline(VulkanDevice& inDevice, const RasterPipelineCreateInfo& inCreateInfo);
        ~VulkanRasterPipeline() override;

        VulkanPipelineLayout* GetPipelineLayout() const;
        VkPipeline GetNative() const;

    private:
        void SavePipelineLayout(const RasterPipelineCreateInfo& inCreateInfo);
        void CreateNativeGraphicsPipeline(const RasterPipelineCreateInfo& inCreateInfo);

        VulkanDevice& device;
        VulkanPipelineLayout* pipelineLayout;
        VkPipeline nativePipeline = VK_NULL_HANDLE;
    };

}