//
// Created by Zach Lee on 2022/4/2.
//

#include <RHI/Vulkan/Pipeline.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/ShaderModule.h>
#include <RHI/Vulkan/PipelineLayout.h>
#include <RHI/Vulkan/Common.h>
#include <unordered_map>

namespace RHI::Vulkan {

    static const char* GetShaderEntry(vk::ShaderStageFlagBits stage)
    {
        static std::unordered_map<vk::ShaderStageFlagBits, const char*> ENTRY_MAP = {
            {vk::ShaderStageFlagBits::eVertex, "VSMain"},
            {vk::ShaderStageFlagBits::eFragment, "FSMain"}
        };
        auto iter = ENTRY_MAP.find(stage);
        Assert(iter != ENTRY_MAP.end() && "invalid shader stage");
        return iter->second;
    }

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

    static vk::PipelineDepthStencilStateCreateInfo ConstructDepthStencil(const GraphicsPipelineCreateInfo& createInfo)
    {
        auto& dsState = createInfo.depthStencilState;
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

    static vk::PipelineInputAssemblyStateCreateInfo ConstructInputAssembly(const GraphicsPipelineCreateInfo& createInfo)
    {
        vk::PipelineInputAssemblyStateCreateInfo assemblyInfo = {};
        assemblyInfo.setTopology(VKEnumCast<PrimitiveTopologyType, vk::PrimitiveTopology>(createInfo.primitiveState.topologyType))
            .setPrimitiveRestartEnable(VK_FALSE);

        return assemblyInfo;
    }

    static vk::PipelineRasterizationStateCreateInfo ConstructRasterization(const GraphicsPipelineCreateInfo& createInfo)
    {
        vk::PipelineRasterizationStateCreateInfo rasterState = {};
        rasterState.setCullMode(VKEnumCast<CullMode, vk::CullModeFlagBits>(createInfo.primitiveState.cullMode))
            .setFrontFace(createInfo.primitiveState.frontFace == FrontFace::CW ? vk::FrontFace::eClockwise : vk::FrontFace::eCounterClockwise)
            .setDepthBiasClamp(createInfo.depthStencilState.depthBiasClamp)
            .setDepthBiasSlopeFactor(createInfo.depthStencilState.depthBiasSlopeScale)
            .setDepthBiasEnable(createInfo.depthStencilState.depthBias == 0 ? VK_FALSE : VK_TRUE)
            .setDepthBiasConstantFactor(static_cast<float>(createInfo.depthStencilState.depthBias))
            .setLineWidth(1.0);

        // TODO DepthClampEnable requires check depth clamping feature
        rasterState.setDepthClampEnable(VK_FALSE);
        // rasterState.setDepthClampEnable(createInfo.primitive.depthClip ? VK_FALSE : VK_TRUE);
        // TODO DepthClipEnable requires VK_EXT_depth_clip_enable

        return rasterState;
    }

    static vk::PipelineMultisampleStateCreateInfo ConstructMultiSampleState(const GraphicsPipelineCreateInfo& createInfo)
    {
        vk::PipelineMultisampleStateCreateInfo multiSampleInfo = {};
        multiSampleInfo.setAlphaToCoverageEnable(createInfo.multiSampleState.alphaToCoverage)
            .setPSampleMask(&createInfo.multiSampleState.mask)
            .setRasterizationSamples(static_cast<vk::SampleCountFlagBits>(createInfo.multiSampleState.count));
        return multiSampleInfo;
    }

    static vk::PipelineViewportStateCreateInfo ConstructViewportInfo(const GraphicsPipelineCreateInfo&)
    {
        vk::PipelineViewportStateCreateInfo viewportState = {};
        viewportState.setViewportCount(1)
            .setPViewports(nullptr)
            .setScissorCount(1)
            .setPScissors(nullptr);
        return viewportState;
    }

    static vk::PipelineColorBlendStateCreateInfo ConstructAttachmentInfo(const GraphicsPipelineCreateInfo& createInfo, std::vector<vk::PipelineColorBlendAttachmentState>& blendStates)
    {
        blendStates.resize(createInfo.fragmentState.colorTargetNum);
        vk::PipelineColorBlendStateCreateInfo colorInfo = {};
        for (uint8_t i = 0; i < createInfo.fragmentState.colorTargetNum; ++i) {
            vk::PipelineColorBlendAttachmentState& blendState = blendStates[i];
            auto& srcState = createInfo.fragmentState.colorTargets[i];
            blendState.setBlendEnable(true)
                .setColorWriteMask(static_cast<vk::ColorComponentFlags>(srcState.writeFlags.Value()))
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

    static vk::PipelineVertexInputStateCreateInfo ConstructVertexInput(const GraphicsPipelineCreateInfo& createInfo,
        std::vector<vk::VertexInputAttributeDescription>& attributes,
        std::vector<vk::VertexInputBindingDescription>& bindings)
    {
        auto vs = static_cast<VKShaderModule*>(createInfo.vertexShader);
        auto& locationTable = vs->GetLocationTable();

        vk::PipelineVertexInputStateCreateInfo vtxInput = {};

        bindings.resize(createInfo.vertexState.bufferLayoutNum);
        for (uint32_t i = 0; i < createInfo.vertexState.bufferLayoutNum; ++i) {
            auto &binding = createInfo.vertexState.bufferLayouts[i];
            bindings[i].binding = i;
            bindings[i].inputRate = binding.stepMode == VertexStepMode::PER_INSTANCE ? vk::VertexInputRate::eInstance
                                                                                     : vk::VertexInputRate::eVertex;
            bindings[i].stride = binding.stride;

            for (uint32_t j = 0; j < binding.attributeNum; ++j) {
                vk::VertexInputAttributeDescription desc = {};
                auto inputName = std::string("in.var.") + std::string(binding.attributes[j].semanticName);
                auto iter = locationTable.find(inputName);
                Assert(iter != locationTable.end());

                desc.setBinding(i)
                    .setLocation(iter->second)
                    .setOffset(binding.attributes[j].offset)
                    .setFormat(VKEnumCast<VertexFormat, vk::Format>(binding.attributes[j].format));
                attributes.emplace_back(desc);
            }
        }
        vtxInput.setVertexAttributeDescriptions(attributes)
            .setVertexBindingDescriptions(bindings);
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
            device.GetVkDevice().destroyRenderPass(renderPass, nullptr);
        }

        if (pipeline) {
            device.GetVkDevice().destroyPipeline(pipeline, nullptr);
        }
    }

    void VKGraphicsPipeline::Destroy()
    {
        delete this;
    }

    void VKGraphicsPipeline::CreateNativeRenderPass(const GraphicsPipelineCreateInfo& createInfo)
    {
        std::vector<vk::SubpassDescription> subPasses;
        std::vector<vk::AttachmentDescription> attachments;
        std::vector<vk::AttachmentReference> colors;
        vk::AttachmentReference depthStencil;

        {
            vk::SubpassDescription subPassInfo = {};
            subPassInfo.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);

            vk::SampleCountFlagBits sampleCount = static_cast<vk::SampleCountFlagBits>(createInfo.multiSampleState.count);

            for (uint32_t i = 0; i < createInfo.fragmentState.colorTargetNum; ++i) {
                auto color = createInfo.fragmentState.colorTargets[i];
                vk::AttachmentDescription desc = {};
                desc.setFormat(VKEnumCast<PixelFormat, vk::Format>(color.format))
                    .setSamples(sampleCount)
                    .setLoadOp(vk::AttachmentLoadOp::eDontCare)
                    .setStoreOp(vk::AttachmentStoreOp::eDontCare)
                    .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
                    .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
                    .setInitialLayout(vk::ImageLayout::eUndefined)
                    .setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal);

                vk::AttachmentReference ref = {};
                ref.setLayout(vk::ImageLayout::eColorAttachmentOptimal)
                    .setAttachment(static_cast<uint32_t>(attachments.size()));
                colors.emplace_back(ref);
                attachments.emplace_back(std::move(desc));
            }
            if (!colors.empty()) {
                subPassInfo.setColorAttachments(colors);
            }

            if (createInfo.depthStencilState.depthEnable || createInfo.depthStencilState.stencilEnable) {
                vk::AttachmentDescription desc = {};
                desc.setFormat(VKEnumCast<PixelFormat, vk::Format>(createInfo.depthStencilState.format))
                    .setSamples(sampleCount)
                    .setInitialLayout(vk::ImageLayout::eUndefined)
                    .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
                depthStencil.setLayout(vk::ImageLayout::eColorAttachmentOptimal)
                    .setAttachment(static_cast<uint32_t>(attachments.size()));
                attachments.emplace_back(std::move(desc));

                subPassInfo.setPDepthStencilAttachment(&depthStencil);
            }
            subPasses.emplace_back(std::move(subPassInfo));
        }

        vk::RenderPassCreateInfo passInfo = {};
        passInfo.setAttachments(attachments)
            .setSubpasses(subPasses)
            .setDependencyCount(0)
            .setPDependencies(nullptr);

        auto result = device.GetVkDevice().createRenderPass(passInfo, nullptr);
        Assert(!!result);
        renderPass = result;
    }

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
        std::vector<vk::PipelineShaderStageCreateInfo> stages;
        auto setStage = [&stages](ShaderModule* module, vk::ShaderStageFlagBits stage) {
            if (module == nullptr) return;
            vk::PipelineShaderStageCreateInfo stageInfo = {};
            stageInfo.setModule(static_cast<const VKShaderModule*>(module)->GetVkShaderModule())
                .setPName(GetShaderEntry(stage))
                .setStage(stage);
            stages.emplace_back(std::move(stageInfo));
        };
        setStage(createInfo.vertexShader, vk::ShaderStageFlagBits::eVertex);
        setStage(createInfo.pixelShader, vk::ShaderStageFlagBits::eFragment);

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

        //As for dynamic rendering, we no longer need to set a render pass
        //instead, create info to define color、depth and stencil attachment at pipeline create time
        std::vector<vk::Format> pixelFormats(createInfo.fragmentState.colorTargetNum);
        for (size_t i = 0; i < createInfo.fragmentState.colorTargetNum; i++)
        {
            auto format = createInfo.fragmentState.colorTargets[i].format;
            pixelFormats[i] = VKEnumCast<PixelFormat, vk::Format>(format);
        }
        vk::PipelineRenderingCreateInfo pipelineRenderingCreateInfo;
        pipelineRenderingCreateInfo.setColorAttachmentCount(createInfo.fragmentState.colorTargetNum)
            .setPColorAttachmentFormats(pixelFormats.data())
            .setDepthAttachmentFormat(VKEnumCast<PixelFormat, vk::Format>(createInfo.depthStencilState.format))
            .setStencilAttachmentFormat(VKEnumCast<PixelFormat, vk::Format>(createInfo.depthStencilState.format));

        vk::GraphicsPipelineCreateInfo pipelineCreateInfo = {};
        pipelineCreateInfo.setStages(stages)
            .setLayout(static_cast<const VKPipelineLayout*>(createInfo.layout)->GetVkPipelineLayout())
            .setPDynamicState(&dynStateInfo)
            .setPMultisampleState(&multiSampleInfo)
            .setPDepthStencilState(&dsInfo)
            .setPInputAssemblyState(&assemblyInfo)
            .setPRasterizationState(&rasterState)
            .setPViewportState(&viewportState)
            .setPTessellationState(nullptr)
            .setPColorBlendState(&colorInfo)
            .setPVertexInputState(&vtxInput)
            .setPNext(&pipelineRenderingCreateInfo);

        auto result =  device.GetVkDevice().createGraphicsPipeline(VK_NULL_HANDLE,
            pipelineCreateInfo, nullptr);
        Assert(result.result == vk::Result::eSuccess);
        pipeline = result.value;
    }

    vk::Pipeline VKGraphicsPipeline::GetVkPipeline()
    {
        return pipeline;
    }
    vk::RenderPass VKGraphicsPipeline::GetVkRenderPass()
    {
        return renderPass;
    }

}