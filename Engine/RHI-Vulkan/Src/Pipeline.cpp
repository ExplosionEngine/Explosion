//
// Created by Zach Lee on 2022/4/2.
//

#include <RHI/Vulkan/Pipeline.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/ShaderModule.h>
#include <RHI/Vulkan/PipelineLayout.h>
#include "RHI/Vulkan/Common.h"

namespace RHI::Vulkan {

    VKGraphicsPipeline::VKGraphicsPipeline(VKDevice& dev, const GraphicsPipelineCreateInfo* createInfo)
        : device(dev), GraphicsPipeline(createInfo)
    {
        CreateNativeGraphicsPipeline(createInfo);
    }

    VKGraphicsPipeline::~VKGraphicsPipeline()
    {
        if (pipeline) {
            device.GetVkDevice().destroyPipeline(pipeline, nullptr);
        }
    }

    void VKGraphicsPipeline::Destroy()
    {
        delete this;
    }

    void VKGraphicsPipeline::CreateNativeGraphicsPipeline(const GraphicsPipelineCreateInfo* createInfo)
    {
        std::vector<vk::PipelineShaderStageCreateInfo> stages;
        auto setStage = [&stages](ShaderModule* module, vk::ShaderStageFlagBits stage) {
            if (module == nullptr) return;
            vk::PipelineShaderStageCreateInfo stageInfo = {};
            stageInfo.setModule(static_cast<const VKShaderModule*>(module)->GetNativeHandle())
                .setStage(vk::ShaderStageFlagBits::eVertex);
            stages.emplace_back(std::move(stageInfo));
        };

        std::vector<vk::DynamicState> dynamicStates = {
            vk::DynamicState::eViewport,
            vk::DynamicState::eScissor
        };
        vk::PipelineDynamicStateCreateInfo dynStateInfo = {};
        dynStateInfo.setDynamicStates(dynamicStates);

        vk::PipelineMultisampleStateCreateInfo multiSampleInfo = {};
        multiSampleInfo.setAlphaToCoverageEnable(createInfo->multiSample.alphaToCoverage)
            .setPSampleMask(&createInfo->multiSample.mask)
            .setRasterizationSamples(static_cast<vk::SampleCountFlagBits>(createInfo->multiSample.count));

        vk::PipelineDepthStencilStateCreateInfo dsInfo = {};
        dsInfo.setDepthTestEnable(createInfo->depthStencil.depthEnable)
            .setDepthWriteEnable(createInfo->depthStencil.depthEnable)
            .setStencilTestEnable(createInfo->depthStencil.stencilEnable);
        // TODO DepthStencil front back

        vk::PipelineInputAssemblyStateCreateInfo assemblyInfo = {};
        assemblyInfo.setTopology(VKEnumCast<PrimitiveTopology, vk::PrimitiveTopology>(createInfo->primitive.topology))
            .setPrimitiveRestartEnable(VK_FALSE);

        vk::PipelineRasterizationStateCreateInfo rasterState = {};
        rasterState.setCullMode(VKEnumCast<CullMode, vk::CullModeFlagBits>(createInfo->primitive.cullMode))
            .setFrontFace(createInfo->primitive.frontFace == FrontFace::CW ? vk::FrontFace::eClockwise : vk::FrontFace::eCounterClockwise)
            .setDepthBiasClamp(createInfo->depthStencil.depthBiasClamp)
            .setDepthBiasSlopeFactor(createInfo->depthStencil.depthBiasSlopeScale)
            .setDepthBiasEnable(createInfo->depthStencil.depthBias == 0 ? VK_FALSE : VK_TRUE)
            .setDepthBiasConstantFactor(static_cast<float>(createInfo->depthStencil.depthBias))
            .setDepthClampEnable(createInfo->primitive.depthClip ? VK_FALSE : VK_TRUE);
        // TODO DepthClipEnable requires VK_EXT_depth_clip_enable

        vk::PipelineViewportStateCreateInfo viewportState = {};
        viewportState.setScissorCount(1)
            .setPViewports(nullptr)
            .setScissorCount(1)
            .setPScissors(nullptr);

        std::vector<vk::PipelineColorBlendAttachmentState> blendStates(createInfo->fragment.colorTargetNum);
        for (uint8_t i = 0; i < createInfo->fragment.colorTargetNum; ++i) {
            vk::PipelineColorBlendAttachmentState& blendState = blendStates[i];
            auto& srcState = createInfo->fragment.colorTargets[i];
            blendState.setBlendEnable(false);
            // TODO BlendEnable ?
        }
        vk::PipelineColorBlendStateCreateInfo colorInfo = {};
        colorInfo.setAttachments(blendStates)
            .setBlendConstants({0, 0, 0, 0})
            .setLogicOpEnable(VK_FALSE)
            .setLogicOp(vk::LogicOp::eClear);

        setStage(createInfo->vertexShader, vk::ShaderStageFlagBits::eVertex);
        setStage(createInfo->fragmentShader, vk::ShaderStageFlagBits::eFragment);

        std::vector<vk::VertexInputAttributeDescription> attributes;
        std::vector<vk::VertexInputBindingDescription> bindings;
        vk::PipelineVertexInputStateCreateInfo vtxInput = {};

        // TODO RenderPass

        vk::GraphicsPipelineCreateInfo pipelineCreateInfo = {};
        pipelineCreateInfo.setStages(stages)
            .setLayout(static_cast<const VKPipelineLayout*>(createInfo->layout)->GetNativeHandle())
            .setPDynamicState(&dynStateInfo)
            .setPMultisampleState(&multiSampleInfo)
            .setPDepthStencilState(&dsInfo)
            .setPInputAssemblyState(&assemblyInfo)
            .setPRasterizationState(&rasterState)
            .setPViewportState(&viewportState)
            .setPTessellationState(nullptr)
            .setPColorBlendState(&colorInfo)
            .setPVertexInputState(&vtxInput);
    }

}