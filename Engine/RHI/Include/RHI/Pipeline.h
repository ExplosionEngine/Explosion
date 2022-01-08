//
// Created by johnk on 8/1/2022.
//

#ifndef EXPLOSION_RHI_PIPELINE_H
#define EXPLOSION_RHI_PIPELINE_H

#include <Common/Utility.h>
#include <RHI/Enum.h>

namespace RHI {
    class PipelineLayout;

    struct PipelineVertexInputBinding {
        size_t binding;
        size_t stride;
        VertexInputRate inputRate;
    };

    struct PipelineVertexInputAttribute {
        size_t binding;
        size_t location;
        size_t offset;
        PixelFormat format;
    };

    struct PipelineVertexInputState {
        size_t bindingNum;
        const PipelineVertexInputBinding* bindings;
        size_t attributeNum;
        const PipelineVertexInputAttribute* attributes;
    };

    struct PipelineViewportState {
        size_t viewportNum;
        const Viewport* viewports;
        size_t scissorNum;
        const Scissor* scissors;
    };

    struct PipelineRasterizerState {
        bool depthClamp;
        bool rasterizerDiscard;
        PolygonMode polygonMode;
        float lineWidth;
        CullMode cullMode;
        FrontFace frontFace;
    };

    struct PipelineMultiSampleState {
        bool sampleShading;
        size_t samples;
    };

    struct PipelineDepthStencilState {
        // TODO
    };

    struct PipelineColorBlendAttachmentState {
        bool colorBlend;
        ColorComponentFlags colorWriteFlags;
        BlendFactor srcColorBlendFactor;
        BlendFactor dstColorBlendFactor;
        BlendOp colorBlendOp;
        BlendFactor srcAlphaBlendFactor;
        BlendFactor dstAlphaBlendFactor;
        BlendOp alphaBlendOp;
    };

    struct PipelineColorBlendState {
        size_t attachmentStateNum;
        PipelineColorBlendAttachmentState attachmentStates;
    };

    struct PipelineDynamicState {
        size_t dynamicStateNum;
        DynamicState dynamicStates;
    };

    struct GraphicsPipelineCreateInfo {
        const PipelineVertexInputState* vertexInputState;
        const PipelineViewportState* viewportState;
        const PipelineRasterizerState* rasterizerState;
        const PipelineMultiSampleState* multiSampleState;
        const PipelineDepthStencilState* depthStencilState;
        const PipelineColorBlendState* colorBlendState;
        const PipelineDynamicState* dynamicState;
        const PipelineLayout* layout;
        PrimitiveTopology topology;
    };

    class Pipeline {
    public:
        NON_COPYABLE(Pipeline)
        virtual ~Pipeline();

    protected:
        explicit Pipeline(const GraphicsPipelineCreateInfo* createInfo);
    };
}

#endif //EXPLOSION_RHI_PIPELINE_H
