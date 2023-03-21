//
// Created by Zach Lee on 2022/4/2.
//
#include <RHI/Vulkan/PipelineLayout.h>
#include <RHI/Vulkan/BindGroupLayout.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Common.h>

namespace RHI::Vulkan {

    VKPipelineLayout::VKPipelineLayout(VKDevice& dev, const PipelineLayoutCreateInfo* createInfo)
        : device(dev), PipelineLayout(createInfo)
    {
        CreateNativePipelineLayout(createInfo);
    }

    VKPipelineLayout::~VKPipelineLayout()
    {
        if (pipelineLayout) {
            device.GetVkDevice().destroyPipelineLayout(pipelineLayout, nullptr);
        }
    }

    void VKPipelineLayout::Destroy()
    {
        delete this;
    }

    vk::PipelineLayout VKPipelineLayout::GetVkPipelineLayout() const
    {
        return pipelineLayout;
    }

    void VKPipelineLayout::CreateNativePipelineLayout(const PipelineLayoutCreateInfo* createInfo)
    {
        std::vector<vk::DescriptorSetLayout> setLayouts(createInfo->bindGroupNum);
        for (uint32_t i = 0; i < createInfo->bindGroupNum; ++i) {
            auto vulkanBindGroup = static_cast<const VKBindGroupLayout*>(createInfo->bindGroupLayouts[i]);
            setLayouts[i] = vulkanBindGroup->GetVkDescriptorSetLayout();
        }

        std::vector<vk::PushConstantRange> pushConstants(createInfo->pipelineConstantLayoutNum);
        for (uint32_t i = 0; i < createInfo->pipelineConstantLayoutNum; ++i) {
            auto& constantInfo = createInfo->pipelineConstantLayouts[i];
            pushConstants[i].setStageFlags(FromRHI(constantInfo.stageFlags))
                .setOffset(constantInfo.offset)
                .setSize(constantInfo.size);
        }

        vk::PipelineLayoutCreateInfo plInfo= {};
        plInfo.setSetLayouts(setLayouts)
            .setPushConstantRanges(pushConstants);
        Assert(device.GetVkDevice().createPipelineLayout(&plInfo, nullptr, &pipelineLayout) == vk::Result::eSuccess);
    }

}