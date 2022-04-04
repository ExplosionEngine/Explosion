//
// Created by Zach Lee on 2022/4/2.
//

#include <RHI/Vulkan/Pipeline.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/ShaderModule.h>
#include <RHI/Vulkan/PipelineLayout.h>
#include <RHI/Vulkan/Common.h>

namespace RHI::Vulkan {

    static vk::StencilOpState ConvertStencilOp(const StencilFaceState& stencilOp, uint32_t readMask, uint32_t writeMask)
    {
        vk::StencilOpState state = {};
        state.setCompareOp(VKEnumCast<ComparisonFunc, vk::CompareOp>(stencilOp.comparisonFunc))
            .setDepthFailOp(VKEnumCast<StencilOp, vk::StencilOp>(stencilOp.depthFailOp))
            .setFailOp(VKEnumCast<StencilOp, vk::StencilOp>(stencilOp.failOp))
            .setPassOp(VKEnumCast<StencilOp, vk::StencilOp>(stencilOp.passOp))
            .setCompareMask(readMask)
            .setWriteMask(writeMask)
            .setReference(0);
        return state;
    }

    static vk::PipelineDepthStencilStateCreateInfo ConstructDepthStencil(const GraphicsPipelineCreateInfo* createInfo)
    {
        auto& dsState = createInfo->depthStencil;
        vk::PipelineDepthStencilStateCreateInfo dsInfo = {};
        dsInfo.setDepthTestEnable(dsState.depthEnable)
            .setDepthWriteEnable(dsState.depthEnable)
            .setStencilTestEnable(dsState.stencilEnable)
            .setFront(ConvertStencilOp(dsState.stencilFront, dsState.stencilReadMask, dsState.stencilWriteMask))
            .setBack(ConvertStencilOp(dsState.stencilBack, dsState.stencilReadMask, dsState.stencilWriteMask))
            .setMinDepthBounds(-1.f)
            .setMaxDepthBounds(1.f)
            .setDepthBoundsTestEnable(VK_FALSE)
            .setDepthCompareOp(VKEnumCast<ComparisonFunc, vk::CompareOp>(dsState.depthComparisonFunc));
        return dsInfo;
    }

    static vk::PipelineInputAssemblyStateCreateInfo ConstructInputAssembly(const GraphicsPipelineCreateInfo* createInfo)
    {
        vk::PipelineInputAssemblyStateCreateInfo assemblyInfo = {};
        assemblyInfo.setTopology(VKEnumCast<PrimitiveTopology, vk::PrimitiveTopology>(createInfo->primitive.topology))
            .setPrimitiveRestartEnable(VK_FALSE);

        return assemblyInfo;
    }

    static vk::PipelineRasterizationStateCreateInfo ConstructRasterization(const GraphicsPipelineCreateInfo* createInfo)
    {
        vk::PipelineRasterizationStateCreateInfo rasterState = {};
        rasterState.setCullMode(VKEnumCast<CullMode, vk::CullModeFlagBits>(createInfo->primitive.cullMode))
            .setFrontFace(createInfo->primitive.frontFace == FrontFace::CW ? vk::FrontFace::eClockwise : vk::FrontFace::eCounterClockwise)
            .setDepthBiasClamp(createInfo->depthStencil.depthBiasClamp)
            .setDepthBiasSlopeFactor(createInfo->depthStencil.depthBiasSlopeScale)
            .setDepthBiasEnable(createInfo->depthStencil.depthBias == 0 ? VK_FALSE : VK_TRUE)
            .setDepthBiasConstantFactor(static_cast<float>(createInfo->depthStencil.depthBias))
            .setDepthClampEnable(createInfo->primitive.depthClip ? VK_FALSE : VK_TRUE);
        // TODO DepthClipEnable requires VK_EXT_depth_clip_enable

        return rasterState;
    }

    static vk::PipelineMultisampleStateCreateInfo ConstructMultiSampleState(const GraphicsPipelineCreateInfo* createInfo)
    {
        vk::PipelineMultisampleStateCreateInfo multiSampleInfo = {};
        multiSampleInfo.setAlphaToCoverageEnable(createInfo->multiSample.alphaToCoverage)
            .setPSampleMask(&createInfo->multiSample.mask)
            .setRasterizationSamples(static_cast<vk::SampleCountFlagBits>(createInfo->multiSample.count));
        return multiSampleInfo;
    }

    static vk::PipelineViewportStateCreateInfo ConstructViewportInfo(const GraphicsPipelineCreateInfo*)
    {
        vk::PipelineViewportStateCreateInfo viewportState = {};
        viewportState.setScissorCount(1)
            .setPViewports(nullptr)
            .setScissorCount(1)
            .setPScissors(nullptr);
        return viewportState;
    }

    static vk::PipelineColorBlendStateCreateInfo ConstructAttachmentInfo(const GraphicsPipelineCreateInfo* createInfo, std::vector<vk::PipelineColorBlendAttachmentState>& blendStates)
    {
        blendStates.resize(createInfo->fragment.colorTargetNum);
        vk::PipelineColorBlendStateCreateInfo colorInfo = {};
        for (uint8_t i = 0; i < createInfo->fragment.colorTargetNum; ++i) {
            vk::PipelineColorBlendAttachmentState& blendState = blendStates[i];
            auto& srcState = createInfo->fragment.colorTargets[i];
            blendState.setBlendEnable(true)
                .setColorWriteMask(static_cast<vk::ColorComponentFlags>(srcState.writeFlags))
                .setAlphaBlendOp(VKEnumCast<BlendOp, vk::BlendOp>(srcState.blend.color.op))
                .setAlphaBlendOp(VKEnumCast<BlendOp, vk::BlendOp>(srcState.blend.alpha.op))
                .setSrcColorBlendFactor(VKEnumCast<BlendFactor, vk::BlendFactor>(srcState.blend.color.srcFactor))
                .setSrcAlphaBlendFactor(VKEnumCast<BlendFactor, vk::BlendFactor>(srcState.blend.alpha.srcFactor))
                .setDstColorBlendFactor(VKEnumCast<BlendFactor, vk::BlendFactor>(srcState.blend.color.dstFactor))
                .setDstAlphaBlendFactor(VKEnumCast<BlendFactor, vk::BlendFactor>(srcState.blend.alpha.dstFactor));
        }
        colorInfo.setAttachments(blendStates)
            .setBlendConstants({0, 0, 0, 0})
            .setLogicOpEnable(VK_FALSE)
            .setLogicOp(vk::LogicOp::eClear);
        return colorInfo;
    }

    static vk::PipelineVertexInputStateCreateInfo ConstructVertexInput(const GraphicsPipelineCreateInfo* createInfo,
        std::vector<vk::VertexInputAttributeDescription>& attributes,
        std::vector<vk::VertexInputBindingDescription>& bindings)
    {
        vk::PipelineVertexInputStateCreateInfo vtxInput = {};

        bindings.resize(createInfo->vertex.bufferLayoutNum);
        for (uint32_t i = 0; i < createInfo->vertex.bufferLayoutNum; ++i) {
            auto &binding = createInfo->vertex.bufferLayouts[i];
            bindings[i].binding = i;
            bindings[i].inputRate = binding.stepMode == VertexStepMode::PER_INSTANCE ? vk::VertexInputRate::eInstance
                                                                                     : vk::VertexInputRate::eVertex;
            bindings[i].stride = binding.stride;

            for (uint32_t j = 0; j < binding.attributeNum; ++j) {
                vk::VertexInputAttributeDescription desc = {};
                desc.setBinding(i)
                    .setLocation(binding.attributes[j].location)
                    .setOffset(binding.attributes[j].offset)
                    .setFormat(VKEnumCast<VertexFormat, vk::Format>(binding.attributes[j].format));
                attributes.emplace_back(desc);
            }
        }
        vtxInput.setVertexAttributeDescriptions(attributes)
            .setVertexBindingDescriptions(bindings);
        return vtxInput;
    }

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
        setStage(createInfo->vertexShader, vk::ShaderStageFlagBits::eVertex);
        setStage(createInfo->fragmentShader, vk::ShaderStageFlagBits::eFragment);

        std::vector<vk::DynamicState> dynamicStates = {
            vk::DynamicState::eViewport,
            vk::DynamicState::eScissor
        };
        vk::PipelineDynamicStateCreateInfo dynStateInfo = {};
        dynStateInfo.setDynamicStates(dynamicStates);

        vk::PipelineMultisampleStateCreateInfo multiSampleInfo = ConstructMultiSampleState(createInfo);
        vk::PipelineDepthStencilStateCreateInfo dsInfo = ConstructDepthStencil(createInfo);
        vk::PipelineInputAssemblyStateCreateInfo assemblyInfo = ConstructInputAssembly(createInfo);
        vk::PipelineRasterizationStateCreateInfo rasterState = ConstructRasterization(createInfo);
        vk::PipelineViewportStateCreateInfo viewportState = ConstructViewportInfo(createInfo);

        std::vector<vk::PipelineColorBlendAttachmentState> blendStates;
        vk::PipelineColorBlendStateCreateInfo colorInfo = ConstructAttachmentInfo(createInfo, blendStates);

        std::vector<vk::VertexInputAttributeDescription> attributes;
        std::vector<vk::VertexInputBindingDescription> bindings;
        vk::PipelineVertexInputStateCreateInfo vtxInput = ConstructVertexInput(createInfo, attributes, bindings);

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

        auto result =  device.GetVkDevice().createGraphicsPipeline(VK_NULL_HANDLE,
            pipelineCreateInfo, nullptr);
        if (result.result != vk::Result::eSuccess) {
            throw VKException("failed to create pipeline");
        }

        pipeline = result.value;
    }

}