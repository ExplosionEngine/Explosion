//
// Created by johnk on 19/2/2022.
//

#pragma once

#include <string>

#include <Common/Utility.h>
#include <RHI/Common.h>

namespace RHI {
    class BindGroupLayout;
    class PipelineLayout;
    class ShaderModule;

    struct VertexAttribute {
        VertexFormat format;
        size_t offset;
        // DirectX 12 using SemanticName and SemanticIndex to specific vertex layout, Vulkan using SPRI-V Reflection to get location
        std::string semanticName;
        uint8_t semanticIndex;

        VertexAttribute();
        VertexAttribute& Format(VertexFormat inFormat);
        VertexAttribute& Offset(size_t inOffset);
        VertexAttribute& SemanticName(std::string inSemanticName);
        VertexAttribute& SemanticIndex(uint8_t inSemanticIndex);
    };

    struct VertexBufferLayout {
        size_t stride;
        VertexStepMode stepMode;
        std::vector<VertexAttribute> attributes;

        VertexBufferLayout();
        VertexBufferLayout& Stride(size_t inStride);
        VertexBufferLayout& StepMode(VertexStepMode inStepMode);
        VertexBufferLayout& Attribute(const VertexAttribute& inAttribute);
    };

    struct VertexState {
        std::vector<VertexBufferLayout> bufferLayouts;

        VertexState();
        VertexState& VertexBufferLayout(const VertexBufferLayout& inLayout);
    };

    struct PrimitiveState {
        // TODO fill mode ?
        PrimitiveTopologyType topologyType;
        IndexFormat stripIndexFormat;
        FrontFace frontFace;
        CullMode cullMode;
        bool depthClip = false;

        PrimitiveState();
        PrimitiveState& TopologyType(PrimitiveTopologyType inTopologyType);
        PrimitiveState& StripIndexFormat(IndexFormat inFormat);
        PrimitiveState& FrontFace(FrontFace inFrontFace);
        PrimitiveState& CullMode(CullMode inCullMode);
        PrimitiveState& DepthClip(bool inDepthClip);
    };

    struct StencilFaceState {
        ComparisonFunc comparisonFunc;
        StencilOp failOp;
        StencilOp depthFailOp;
        StencilOp passOp;

        StencilFaceState();
        StencilFaceState& ComparisonFunc(ComparisonFunc inFunc);
        StencilFaceState& FailOp(StencilOp inFailOp);
        StencilFaceState& DepthFailOp(StencilOp inDepthFailOp);
        StencilFaceState& PassOp(StencilOp inPassOp);
    };

    struct DepthStencilState {
        bool depthEnable;
        bool stencilEnable;
        PixelFormat format;
        ComparisonFunc depthComparisonFunc;
        StencilFaceState stencilFront;
        StencilFaceState stencilBack;
        uint8_t stencilReadMask;
        uint8_t stencilWriteMask;
        int32_t depthBias;
        float depthBiasSlopeScale;
        float depthBiasClamp;

        DepthStencilState();
        DepthStencilState& DepthEnabled(bool inDepthEnabled);
        DepthStencilState& StencilEnabled(bool inStencilEnabled);
        DepthStencilState& Format(PixelFormat inFormat);
        DepthStencilState& DepthComparisonFunc(ComparisonFunc inFunc);
        DepthStencilState& StencilFront(StencilFaceState inState);
        DepthStencilState& StencilBack(StencilFaceState inState);
        DepthStencilState& StencilReadMask(uint8_t inStencilReadMask);
        DepthStencilState& StencilWriteMask(uint8_t inStencilWriteMask);
        DepthStencilState& DepthBias(int32_t inDepthBias);
        DepthStencilState& DepthBiasSlopeScale(float inDepthBiasSlopeScale);
        DepthStencilState& DepthBiasClamp(float inDepthBiasClamp);
    };

    struct MultiSampleState {
        uint8_t count;
        uint32_t mask;
        bool alphaToCoverage = false;

        MultiSampleState();
        MultiSampleState& Count(uint8_t inCount);
        MultiSampleState& Mask(uint32_t inMask);
        MultiSampleState& AlphaToCoverage(bool inAlphaToCoverage);
    };

    struct BlendComponent {
        BlendOp op;
        BlendFactor srcFactor;
        BlendFactor dstFactor;

        BlendComponent();
        BlendComponent& Op(BlendOp inOp);
        BlendComponent& SrcFactor(BlendFactor inSrcFactor);
        BlendComponent& DstFactor(BlendFactor inDstFactor);
    };

    struct BlendState {
        BlendComponent color;
        BlendComponent alpha;

        BlendState();
        BlendState& Color(BlendComponent inColor);
        BlendState& Alpha(BlendComponent inAlpha);
    };

    struct ColorTargetState {
        PixelFormat format;
        BlendState blend;
        ColorWriteFlags writeFlags;

        ColorTargetState();
        ColorTargetState& Format(PixelFormat inFormat);
        ColorTargetState& Blend(BlendState inBlend);
        ColorTargetState& WriteFlags(ColorWriteFlags inFlags);
    };

    struct FragmentState {
        std::vector<ColorTargetState> colorTargets;

        FragmentState();
        FragmentState& ColorTarget(const ColorTargetState& inState);
    };

    struct ComputePipelineCreateInfo {
        PipelineLayout* layout;
        ShaderModule* computeShader;

        ComputePipelineCreateInfo();
        ComputePipelineCreateInfo& Layout(PipelineLayout* inLayout);
        ComputePipelineCreateInfo& ComputeShader(ShaderModule* inComputeShader);
    };

    struct GraphicsPipelineCreateInfo {
        PipelineLayout* layout;

        ShaderModule* vertexShader;
        ShaderModule* pixelShader;
        ShaderModule* geometryShader;
        ShaderModule* domainShader;
        ShaderModule* hullShader;

        VertexState vertexState;
        PrimitiveState primitiveState;
        DepthStencilState depthStencilState;
        MultiSampleState multiSampleState;
        FragmentState fragmentState;

        std::string debugName;

        GraphicsPipelineCreateInfo();
        GraphicsPipelineCreateInfo& Layout(PipelineLayout* inLayout);
        GraphicsPipelineCreateInfo& VertexShader(ShaderModule* inVertexShader);
        GraphicsPipelineCreateInfo& PixelShader(ShaderModule* inPixelShader);
        GraphicsPipelineCreateInfo& GeometryShader(ShaderModule* inGeometryShader);
        GraphicsPipelineCreateInfo& DomainShader(ShaderModule* inDomainShader);
        GraphicsPipelineCreateInfo& HullShader(ShaderModule* inHullShader);
        GraphicsPipelineCreateInfo& VertexState(const VertexState& inVertexState);
        GraphicsPipelineCreateInfo& PrimitiveState(const PrimitiveState& inPrimitiveState);
        GraphicsPipelineCreateInfo& DepthStencilState(const DepthStencilState& inDepthStencilState);
        GraphicsPipelineCreateInfo& MultiSampleState(const MultiSampleState& inMultiSampleState);
        GraphicsPipelineCreateInfo& FragmentState(const FragmentState& inFragmentState);
        GraphicsPipelineCreateInfo& DebugName(std::string inDebugName);
    };

    class Pipeline {
    public:
        NonCopyable(Pipeline)
        virtual ~Pipeline();

        virtual void Destroy() = 0;

    protected:
        Pipeline();
    };

    class ComputePipeline : public Pipeline {
    public:
        NonCopyable(ComputePipeline)
        ~ComputePipeline() override;

        void Destroy() override = 0;

    protected:
        explicit ComputePipeline(const ComputePipelineCreateInfo& createInfo);
    };

    class GraphicsPipeline : public Pipeline {
    public:
        NonCopyable(GraphicsPipeline)
        ~GraphicsPipeline() override;

        void Destroy() override = 0;

    protected:
        explicit GraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo);
    };
}
