//
// Created by Zach Lee on 2022/4/2.
//

#pragma once

#include <vulkan/vulkan.h>

#include <RHI/Pipeline.h>

namespace RHI::Vulkan {
    class VulkanDevice;
    class VulkanPipelineLayout;

    class VulkanRasterPipeline : public RasterPipeline {
    public:
        NonCopyable(VulkanRasterPipeline)
        VulkanRasterPipeline(VulkanDevice& inDevice, const RasterPipelineCreateInfo& inCreateInfo);
        ~VulkanRasterPipeline() override;

        VulkanPipelineLayout* GetPipelineLayout() const;
        VkPipeline GetNative();

    private:
        void SavePipelineLayout(const RasterPipelineCreateInfo& inCreateInfo);
        void CreateNativeGraphicsPipeline(const RasterPipelineCreateInfo& inCreateInfo);

        VulkanDevice& device;
        VulkanPipelineLayout* pipelineLayout;
        VkPipeline nativePipeline;
    };

    class VulkanComputePipeline : public ComputePipeline {
    public:
        NonCopyable(VulkanComputePipeline)
        VulkanComputePipeline(VulkanDevice& inDevice, const ComputePipelineCreateInfo& inCreateInfo);
        ~VulkanComputePipeline() override;
        void Destroy() override;

        VulkanPipelineLayout* GetPipelineLayout() const;
        VkPipeline GetNative() const;

    private:
        void SavePipelineLayout(const ComputePipelineCreateInfo& inCreateInfo);
        void CreateNativeComputePipeline(const ComputePipelineCreateInfo& inCreateInfo);

        VulkanDevice& device;
        VulkanPipelineLayout* pipelineLayout;
        VkPipeline nativePipeline;
    };

}