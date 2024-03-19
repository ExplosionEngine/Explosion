//
// Created by Zach Lee on 2022/4/2.
//
#include <RHI/Vulkan/PipelineLayout.h>
#include <RHI/Vulkan/BindGroupLayout.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Common.h>

namespace RHI::Vulkan {

    VKPipelineLayout::VKPipelineLayout(VKDevice& dev, const PipelineLayoutCreateInfo& createInfo)
        : device(dev), PipelineLayout(createInfo)
    {
        CreateNativePipelineLayout(createInfo);
    }

    VKPipelineLayout::~VKPipelineLayout()
    {
        if (pipelineLayout) {
            vkDestroyPipelineLayout(device.GetVkDevice(), pipelineLayout, nullptr);
        }
    }

    void VKPipelineLayout::Destroy()
    {
        delete this;
    }

    VkPipelineLayout VKPipelineLayout::GetVkPipelineLayout() const
    {
        return pipelineLayout;
    }

    void VKPipelineLayout::CreateNativePipelineLayout(const PipelineLayoutCreateInfo& createInfo)
    {
        std::vector<VkDescriptorSetLayout> setLayouts(createInfo.bindGroupLayoutNum);
        for (uint32_t i = 0; i < createInfo.bindGroupLayoutNum; ++i) {
            const auto* vulkanBindGroup = static_cast<const VKBindGroupLayout*>(createInfo.bindGroupLayouts[i]);
            setLayouts[i] = vulkanBindGroup->GetVkDescriptorSetLayout();
        }

        std::vector<VkPushConstantRange> pushConstants(createInfo.pipelineConstantLayoutNum);
        for (uint32_t i = 0; i < createInfo.pipelineConstantLayoutNum; ++i) {
            const auto& constantInfo = createInfo.pipelineConstantLayouts[i];
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
        Assert(vkCreatePipelineLayout(device.GetVkDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) == VK_SUCCESS);

#if BUILD_CONFIG_DEBUG
        if (!createInfo.debugName.empty()) {
            device.SetObjectName(VK_OBJECT_TYPE_PIPELINE_LAYOUT, reinterpret_cast<uint64_t>(pipelineLayout), createInfo.debugName.c_str());
        }
#endif
    }

}