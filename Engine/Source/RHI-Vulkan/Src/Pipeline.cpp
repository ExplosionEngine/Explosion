//
// Created by Zach Lee on 2022/4/2.
//

#include <array>
#include <RHI/Vulkan/Pipeline.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/ShaderModule.h>
#include <RHI/Vulkan/PipelineLayout.h>
#include <RHI/Vulkan/Common.h>
#include <unordered_map>

namespace RHI::Vulkan {

    static const char* GetShaderEntry(VkShaderStageFlagBits stage)
    {
        static std::unordered_map<VkShaderStageFlagBits, const char*> ENTRY_MAP = {
            {VK_SHADER_STAGE_VERTEX_BIT, "VSMain"},
            {VK_SHADER_STAGE_FRAGMENT_BIT, "FSMain"}
        };
        auto iter = ENTRY_MAP.find(stage);
        Assert(iter != ENTRY_MAP.end() && "invalid shader stage");
        return iter->second;
    }

    static VkStencilOpState ConvertStencilOp(const StencilFaceState& stencilOp, uint32_t readMask, uint32_t writeMask)
    {
        VkStencilOpState state = {};
        state.compareOp = VKEnumCast<ComparisonFunc, VkCompareOp>(stencilOp.comparisonFunc);
        state.depthFailOp = VKEnumCast<StencilOp, VkStencilOp>(stencilOp.depthFailOp);
        state.failOp = VKEnumCast<StencilOp, VkStencilOp>(stencilOp.failOp);
        state.passOp = VKEnumCast<StencilOp, VkStencilOp>(stencilOp.passOp);
        state.compareMask = readMask;
        state.writeMask = writeMask;
        state.reference = 0;
        return state;
    }

    static VkPipelineDepthStencilStateCreateInfo ConstructDepthStencil(const GraphicsPipelineCreateInfo& createInfo)
    {
        const auto& dsState = createInfo.depthStencilState;
        VkPipelineDepthStencilStateCreateInfo dsInfo = {};
        dsInfo.depthTestEnable = dsState.depthEnable;
        dsInfo.depthWriteEnable = dsState.depthEnable;
        dsInfo.stencilTestEnable = dsState.stencilEnable;
        dsInfo.front = ConvertStencilOp(dsState.stencilFront, dsState.stencilReadMask, dsState.stencilWriteMask);
        dsInfo.back = ConvertStencilOp(dsState.stencilBack, dsState.stencilReadMask, dsState.stencilWriteMask);
        dsInfo.minDepthBounds = -1.f;
        dsInfo.maxDepthBounds = 1.f;
        dsInfo.depthBoundsTestEnable = VK_FALSE;
        dsInfo.depthCompareOp = VKEnumCast<ComparisonFunc, VkCompareOp>(dsState.depthComparisonFunc);
        return dsInfo;
    }

    static VkPipelineInputAssemblyStateCreateInfo ConstructInputAssembly(const GraphicsPipelineCreateInfo& createInfo)
    {
        VkPipelineInputAssemblyStateCreateInfo assemblyInfo = {};
        assemblyInfo.topology = VKEnumCast<PrimitiveTopologyType, VkPrimitiveTopology>(createInfo.primitiveState.topologyType);
        assemblyInfo.primitiveRestartEnable = VK_FALSE;

        return assemblyInfo;
    }

    static VkPipelineRasterizationStateCreateInfo ConstructRasterization(const GraphicsPipelineCreateInfo& createInfo)
    {
        VkPipelineRasterizationStateCreateInfo rasterState = {};
        rasterState.cullMode = VKEnumCast<CullMode, VkCullModeFlagBits>(createInfo.primitiveState.cullMode);
        rasterState.frontFace = createInfo.primitiveState.frontFace == FrontFace::cw ? VK_FRONT_FACE_CLOCKWISE : VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterState.depthBiasClamp = createInfo.depthStencilState.depthBiasClamp;
        rasterState.depthBiasSlopeFactor = createInfo.depthStencilState.depthBiasSlopeScale;
        rasterState.depthBiasEnable = createInfo.depthStencilState.depthBias == 0 ? VK_FALSE : VK_TRUE;
        rasterState.depthBiasConstantFactor = static_cast<float>(createInfo.depthStencilState.depthBias);
        rasterState.lineWidth = 1.0;

        // TODO DepthClampEnable requires check depth clamping feature
        rasterState.depthClampEnable = VK_FALSE;
        // rasterState.setDepthClampEnable(createInfo.primitive.depthClip ? VK_FALSE : VK_TRUE);
        // TODO DepthClipEnable requires VK_EXT_depth_clip_enable

        return rasterState;
    }

    static VkPipelineMultisampleStateCreateInfo ConstructMultiSampleState(const GraphicsPipelineCreateInfo& createInfo)
    {
        VkPipelineMultisampleStateCreateInfo multiSampleInfo = {};
        multiSampleInfo.alphaToCoverageEnable = createInfo.multiSampleState.alphaToCoverage;
        multiSampleInfo.pSampleMask = &createInfo.multiSampleState.mask;
        multiSampleInfo.rasterizationSamples = static_cast<VkSampleCountFlagBits>(createInfo.multiSampleState.count);
        return multiSampleInfo;
    }

    static VkPipelineViewportStateCreateInfo ConstructViewportInfo(const GraphicsPipelineCreateInfo&)
    {
        VkPipelineViewportStateCreateInfo viewportState = {};
        viewportState.viewportCount = 1;
        viewportState.pViewports = nullptr;
        viewportState.scissorCount = 1;
        viewportState.pScissors = nullptr;
        return viewportState;
    }

    static VkPipelineColorBlendStateCreateInfo ConstructAttachmentInfo(const GraphicsPipelineCreateInfo& createInfo, std::vector<VkPipelineColorBlendAttachmentState>& blendStates)
    {
        blendStates.resize(createInfo.fragmentState.colorTargetNum);
        VkPipelineColorBlendStateCreateInfo colorInfo = {};
        for (uint8_t i = 0; i < createInfo.fragmentState.colorTargetNum; ++i) {
            VkPipelineColorBlendAttachmentState& blendState = blendStates[i];
            const auto& srcState = createInfo.fragmentState.colorTargets[i];
            blendState.blendEnable = true;
            blendState.colorWriteMask = static_cast<VkColorComponentFlags>(srcState.writeFlags.Value());
            blendState.alphaBlendOp = VKEnumCast<BlendOp, VkBlendOp>(srcState.blend.color.op);
            blendState.alphaBlendOp = VKEnumCast<BlendOp, VkBlendOp>(srcState.blend.alpha.op);
            blendState.srcColorBlendFactor = VKEnumCast<BlendFactor, VkBlendFactor>(srcState.blend.color.srcFactor);
            blendState.srcAlphaBlendFactor = VKEnumCast<BlendFactor, VkBlendFactor>(srcState.blend.alpha.srcFactor);
            blendState.dstColorBlendFactor = VKEnumCast<BlendFactor, VkBlendFactor>(srcState.blend.color.dstFactor);
            blendState.dstAlphaBlendFactor = VKEnumCast<BlendFactor, VkBlendFactor>(srcState.blend.alpha.dstFactor);
        }

        colorInfo.pAttachments = blendStates.data();
        colorInfo.logicOpEnable = VK_FALSE;
        colorInfo.logicOp = VK_LOGIC_OP_CLEAR;
        colorInfo.blendConstants[0] = 0.0f;
        colorInfo.blendConstants[1] = 0.0f;
        colorInfo.blendConstants[2] = 0.0f;
        colorInfo.blendConstants[3] = 0.0f;
        return colorInfo;
    }

    static VkPipelineVertexInputStateCreateInfo ConstructVertexInput(const GraphicsPipelineCreateInfo& createInfo,
        std::vector<VkVertexInputAttributeDescription>& attributes,
        std::vector<VkVertexInputBindingDescription>& bindings)
    {
        auto* vs = static_cast<VKShaderModule*>(createInfo.vertexShader);
        const auto& locationTable = vs->GetLocationTable();

        VkPipelineVertexInputStateCreateInfo vtxInput = {};

        bindings.resize(createInfo.vertexState.bufferLayoutNum);
        for (uint32_t i = 0; i < createInfo.vertexState.bufferLayoutNum; ++i) {
            const auto& binding = createInfo.vertexState.bufferLayouts[i];
            bindings[i].binding = i;
            bindings[i].inputRate = binding.stepMode == VertexStepMode::perInstance ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX;
            bindings[i].stride = binding.stride;

            for (uint32_t j = 0; j < binding.attributeNum; ++j) {
                VkVertexInputAttributeDescription desc = {};
                auto inputName = std::string("in.var.") + std::string(binding.attributes[j].semanticName);
                auto iter = locationTable.find(inputName);
                Assert(iter != locationTable.end());

                desc.binding = i;
                desc.location = iter->second;
                desc.offset = binding.attributes[j].offset;
                desc.format = VKEnumCast<VertexFormat, VkFormat>(binding.attributes[j].format);
                attributes.emplace_back(desc);
            }
        }
        vtxInput.pVertexAttributeDescriptions = attributes.data();
        vtxInput.pVertexBindingDescriptions = bindings.data();
        return vtxInput;
    }

    VKGraphicsPipeline::VKGraphicsPipeline(VKDevice& dev, const GraphicsPipelineCreateInfo& createInfo)
        : device(dev), GraphicsPipeline(createInfo)
    {
        SavePipelineLayout(createInfo);
        CreateNativeGraphicsPipeline(createInfo);
    }

    VKGraphicsPipeline::~VKGraphicsPipeline()
    {
        if (renderPass) {
            vkDestroyRenderPass(device.GetVkDevice(), renderPass, nullptr);
        }

        if (pipeline) {
            vkDestroyPipeline(device.GetVkDevice(), pipeline, nullptr);
        }
    }

    void VKGraphicsPipeline::Destroy()
    {
        delete this;
    }

//    void VKGraphicsPipeline::CreateNativeRenderPass(const GraphicsPipelineCreateInfo& createInfo)
//    {
//        std::vector<VkSubpassDescription> subPasses;
//        std::vector<VkAttachmentDescription> attachments;
//        std::vector<VkAttachmentReference> colors;
//        VkAttachmentReference depthStencil;
//
//        {
//            vk::SubpassDescription subPassInfo = {};
//            subPassInfo.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);
//
//            vk::SampleCountFlagBits sampleCount = static_cast<vk::SampleCountFlagBits>(createInfo.multiSampleState.count);
//
//            for (uint32_t i = 0; i < createInfo.fragmentState.colorTargetNum; ++i) {
//                auto color = createInfo.fragmentState.colorTargets[i];
//                vk::AttachmentDescription desc = {};
//                desc.setFormat(VKEnumCast<PixelFormat, vk::Format>(color.format))
//                    .setSamples(sampleCount)
//                    .setLoadOp(vk::AttachmentLoadOp::eDontCare)
//                    .setStoreOp(vk::AttachmentStoreOp::eDontCare)
//                    .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
//                    .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
//                    .setInitialLayout(vk::ImageLayout::eUndefined)
//                    .setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal);
//
//                vk::AttachmentReference ref = {};
//                ref.setLayout(vk::ImageLayout::eColorAttachmentOptimal)
//                    .setAttachment(static_cast<uint32_t>(attachments.size()));
//                colors.emplace_back(ref);
//                attachments.emplace_back(std::move(desc));
//            }
//            if (!colors.empty()) {
//                subPassInfo.setColorAttachments(colors);
//            }
//
//            if (createInfo.depthStencilState.depthEnable || createInfo.depthStencilState.stencilEnable) {
//                vk::AttachmentDescription desc = {};
//                desc.setFormat(VKEnumCast<PixelFormat, vk::Format>(createInfo.depthStencilState.format))
//                    .setSamples(sampleCount)
//                    .setInitialLayout(vk::ImageLayout::eUndefined)
//                    .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
//                depthStencil.setLayout(vk::ImageLayout::eColorAttachmentOptimal)
//                    .setAttachment(static_cast<uint32_t>(attachments.size()));
//                attachments.emplace_back(std::move(desc));
//
//                subPassInfo.setPDepthStencilAttachment(&depthStencil);
//            }
//            subPasses.emplace_back(std::move(subPassInfo));
//        }
//
//        vk::RenderPassCreateInfo passInfo = {};
//        passInfo.setAttachments(attachments)
//            .setSubpasses(subPasses)
//            .setDependencyCount(0)
//            .setPDependencies(nullptr);
//
//        auto result = device.GetVkDevice().createRenderPass(passInfo, nullptr);
//        Assert(!!result);
//        renderPass = result;
//    }

    VKPipelineLayout* VKGraphicsPipeline::GetPipelineLayout() const
    {
        return pipelineLayout;
    }

    void VKGraphicsPipeline::SavePipelineLayout(const GraphicsPipelineCreateInfo& createInfo)
    {
        auto* layout = dynamic_cast<VKPipelineLayout*>(createInfo.layout);
        Assert(layout);
        pipelineLayout = layout;
    }

    void VKGraphicsPipeline::CreateNativeGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo)
    {
        std::vector<VkPipelineShaderStageCreateInfo> stages;
        auto setStage = [&stages](ShaderModule* module, VkShaderStageFlagBits stage) {
            if (module == nullptr) {
                return;
            }
            VkPipelineShaderStageCreateInfo stageInfo = {};
            stageInfo.module = static_cast<const VKShaderModule*>(module)->GetVkShaderModule();
            stageInfo.pName = GetShaderEntry(stage);
            stageInfo.stage = stage;
            stages.emplace_back(std::move(stageInfo));
        };
        setStage(createInfo.vertexShader, VK_SHADER_STAGE_VERTEX_BIT);
        setStage(createInfo.pixelShader, VK_SHADER_STAGE_FRAGMENT_BIT);

        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        VkPipelineDynamicStateCreateInfo dynStateInfo = {};
        dynStateInfo.dynamicStateCount = dynamicStates.size();
        dynStateInfo.pDynamicStates = dynamicStates.data();

        VkPipelineMultisampleStateCreateInfo multiSampleInfo = ConstructMultiSampleState(createInfo);
        VkPipelineDepthStencilStateCreateInfo dsInfo = ConstructDepthStencil(createInfo);
        VkPipelineInputAssemblyStateCreateInfo assemblyInfo = ConstructInputAssembly(createInfo);
        VkPipelineRasterizationStateCreateInfo rasterState = ConstructRasterization(createInfo);
        VkPipelineViewportStateCreateInfo viewportState = ConstructViewportInfo(createInfo);

        std::vector<VkPipelineColorBlendAttachmentState> blendStates;
        VkPipelineColorBlendStateCreateInfo colorInfo = ConstructAttachmentInfo(createInfo, blendStates);

        std::vector<VkVertexInputAttributeDescription> attributes;
        std::vector<VkVertexInputBindingDescription> bindings;
        VkPipelineVertexInputStateCreateInfo vtxInput = ConstructVertexInput(createInfo, attributes, bindings);

        std::vector<VkFormat> pixelFormats(createInfo.fragmentState.colorTargetNum);
        for (size_t i = 0; i < createInfo.fragmentState.colorTargetNum; i++)
        {
            auto format = createInfo.fragmentState.colorTargets[i].format;
            pixelFormats[i] = VKEnumCast<PixelFormat, VkFormat>(format);
        }

        VkPipelineRenderingCreateInfo pipelineRenderingCreateInfo;
        pipelineRenderingCreateInfo.colorAttachmentCount = createInfo.fragmentState.colorTargetNum;
        pipelineRenderingCreateInfo.pColorAttachmentFormats = pixelFormats.data();

        if (createInfo.depthStencilState.depthEnable) {
            pipelineRenderingCreateInfo.depthAttachmentFormat = VKEnumCast<PixelFormat, VkFormat>(createInfo.depthStencilState.format);
        }
        if (createInfo.depthStencilState.stencilEnable) {
            pipelineRenderingCreateInfo.stencilAttachmentFormat = VKEnumCast<PixelFormat, VkFormat>(createInfo.depthStencilState.format);
        }

        VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
        pipelineCreateInfo.pStages = stages.data();
        pipelineCreateInfo.layout = static_cast<const VKPipelineLayout*>(createInfo.layout)->GetVkPipelineLayout();
        pipelineCreateInfo.pDynamicState = &dynStateInfo;
        pipelineCreateInfo.pMultisampleState = &multiSampleInfo;
        pipelineCreateInfo.pDepthStencilState = &dsInfo;
        pipelineCreateInfo.pInputAssemblyState = &assemblyInfo;
        pipelineCreateInfo.pRasterizationState = &rasterState;
        pipelineCreateInfo.pViewportState = &viewportState;
        pipelineCreateInfo.pTessellationState = nullptr;
        pipelineCreateInfo.pColorBlendState = &colorInfo;
        pipelineCreateInfo.pVertexInputState = &vtxInput;
        pipelineCreateInfo.pNext = &pipelineRenderingCreateInfo;

        Assert(vkCreateGraphicsPipelines(device.GetVkDevice(), VK_NULL_HANDLE,1, &pipelineCreateInfo, nullptr, &pipeline) == VK_SUCCESS);
    }

    VkPipeline VKGraphicsPipeline::GetVkPipeline()
    {
        return pipeline;
    }
    VkRenderPass VKGraphicsPipeline::GetVkRenderPass()
    {
        return renderPass;
    }

}