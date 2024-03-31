//
// Created by Zach Lee on 2022/4/2.
//
#include <RHI/Vulkan/PipelineLayout.h>
#include <RHI/Vulkan/BindGroupLayout.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Common.h>

namespace RHI::Vulkan {

    VulkanPipelineLayout::VulkanPipelineLayout(VulkanDevice& inDevice, const PipelineLayoutCreateInfo& inCreateInfo)
        : PipelineLayout(inCreateInfo)
        , device(inDevice)
        , nativePipelineLayout(VK_NULL_HANDLE)
    {
        CreateNativePipelineLayout(inCreateInfo);
    }

    VulkanPipelineLayout::~VulkanPipelineLayout()
    {
        if (nativePipelineLayout) {
            vkDestroyPipelineLayout(device.GetNative(), nativePipelineLayout, nullptr);
        }
    }

    void VulkanPipelineLayout::Destroy()
    {
        delete this;
    }

    VkPipelineLayout VulkanPipelineLayout::GetNative() const
    {
        return nativePipelineLayout;
    }

    void VulkanPipelineLayout::CreateNativePipelineLayout(const PipelineLayoutCreateInfo& inCreateInfo)
    {
        std::vector<VkDescriptorSetLayout> setLayouts(inCreateInfo.bindGroupLayouts.size());
        for (uint32_t i = 0; i < inCreateInfo.bindGroupLayouts.size(); ++i) {
            const auto* vulkanBindGroup = static_cast<const VulkanBindGroupLayout*>(inCreateInfo.bindGroupLayouts[i]);
            setLayouts[i] = vulkanBindGroup->GetNative();
        }

        std::vector<VkPushConstantRange> pushConstants(inCreateInfo.pipelineConstantLayouts.size());
        for (uint32_t i = 0; i < inCreateInfo.pipelineConstantLayouts.size(); ++i) {
            const auto& constantInfo = inCreateInfo.pipelineConstantLayouts[i];
            pushConstants[i].stageFlags = VKFlagsCast<ShaderStageFlags, VkShaderStageFlags>(constantInfo.stageFlags);
            pushConstants[i].offset = constantInfo.offset;
            pushConstants[i].size = constantInfo.size;
        }

        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = setLayouts.size();
        pipelineLayoutInfo.pSetLayouts = setLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = pushConstants.size();
        pipelineLayoutInfo.pPushConstantRanges = pushConstants.data();
        Assert(vkCreatePipelineLayout(device.GetNative(), &pipelineLayoutInfo, nullptr, &nativePipelineLayout) == VK_SUCCESS);

#if BUILD_CONFIG_DEBUG
        if (!inCreateInfo.debugName.empty()) {
            device.SetObjectName(VK_OBJECT_TYPE_PIPELINE_LAYOUT, reinterpret_cast<uint64_t>(nativePipelineLayout), inCreateInfo.debugName.c_str());
        }
#endif
    }

}