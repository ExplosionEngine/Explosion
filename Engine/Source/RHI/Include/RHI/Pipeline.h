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
        // DirectX 12 using SemanticName and SemanticIndex to specific vertex layout, Vulkan using SPRI-V Reflection to get location
        std::string semanticName;
        uint8_t semanticIndex;
        VertexFormat format;
        size_t offset;

        VertexAttribute(
            std::string inSemanticName = "",
            uint8_t inSemanticIndex = 0,
            VertexFormat inFormat = VertexFormat::max,
            size_t inOffset = 0);

        VertexAttribute& SetSemanticName(std::string inSemanticName);
        VertexAttribute& SetSemanticIndex(uint8_t inSemanticIndex);
        VertexAttribute& SetFormat(VertexFormat inFormat);
        VertexAttribute& SetOffset(size_t inOffset);
    };

    struct VertexBufferLayout {
        size_t stride;
        VertexStepMode stepMode;
        std::vector<VertexAttribute> attributes;

        VertexBufferLayout();
        VertexBufferLayout& SetStride(size_t inStride);
        VertexBufferLayout& SetStepMode(VertexStepMode inStepMode);
        VertexBufferLayout& AddAttribute(const VertexAttribute& inAttribute);
    };

    struct VertexState {
        std::vector<VertexBufferLayout> bufferLayouts;

        VertexState();
        VertexState& AddVertexBufferLayout(const VertexBufferLayout& inLayout);
    };

    struct PrimitiveState {
        // TODO fill mode ?
        PrimitiveTopologyType topologyType;
        IndexFormat stripIndexFormat;
        FrontFace frontFace;
        CullMode cullMode;
        bool depthClip = false;

        PrimitiveState();
        PrimitiveState& SetTopologyType(PrimitiveTopologyType inTopologyType);
        PrimitiveState& SetStripIndexFormat(IndexFormat inFormat);
        PrimitiveState& SetFrontFace(FrontFace inFrontFace);
        PrimitiveState& SetCullMode(CullMode inCullMode);
        PrimitiveState& SetDepthClip(bool inDepthClip);
    };

    struct StencilFaceState {
        ComparisonFunc comparisonFunc;
        StencilOp failOp;
        StencilOp depthFailOp;
        StencilOp passOp;

        StencilFaceState();
        StencilFaceState& SetComparisonFunc(ComparisonFunc inFunc);
        StencilFaceState& SetFailOp(StencilOp inFailOp);
        StencilFaceState& SetDepthFailOp(StencilOp inDepthFailOp);
        StencilFaceState& SetPassOp(StencilOp inPassOp);
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
        DepthStencilState& SetDepthEnabled(bool inDepthEnabled);
        DepthStencilState& SetStencilEnabled(bool inStencilEnabled);
        DepthStencilState& SetFormat(PixelFormat inFormat);
        DepthStencilState& SetDepthComparisonFunc(ComparisonFunc inFunc);
        DepthStencilState& SetStencilFront(StencilFaceState inState);
        DepthStencilState& SetStencilBack(StencilFaceState inState);
        DepthStencilState& SetStencilReadMask(uint8_t inStencilReadMask);
        DepthStencilState& SetStencilWriteMask(uint8_t inStencilWriteMask);
        DepthStencilState& SetDepthBias(int32_t inDepthBias);
        DepthStencilState& SetDepthBiasSlopeScale(float inDepthBiasSlopeScale);
        DepthStencilState& SetDepthBiasClamp(float inDepthBiasClamp);
    };

    struct MultiSampleState {
        uint8_t count;
        uint32_t mask;
        bool alphaToCoverage = false;

        MultiSampleState(
            uint8_t inCount = 1,
            uint32_t inMask = 0xffffffff,
            bool inAlphaToCoverage = false);

        MultiSampleState& SetCount(uint8_t inCount);
        MultiSampleState& SetMask(uint32_t inMask);
        MultiSampleState& SetAlphaToCoverage(bool inAlphaToCoverage);
    };

    struct BlendComponent {
        BlendOp op;
        BlendFactor srcFactor;
        BlendFactor dstFactor;

        BlendComponent();
        BlendComponent& SetOp(BlendOp inOp);
        BlendComponent& SetSrcFactor(BlendFactor inSrcFactor);
        BlendComponent& SetDstFactor(BlendFactor inDstFactor);
    };

    struct BlendState {
        BlendComponent color;
        BlendComponent alpha;

        BlendState();
        BlendState& SetColor(BlendComponent inColor);
        BlendState& SetAlpha(BlendComponent inAlpha);
    };

    struct ColorTargetState {
        PixelFormat format;
        BlendState blend;
        ColorWriteFlags writeFlags;

        ColorTargetState();
        ColorTargetState& SetFormat(PixelFormat inFormat);
        ColorTargetState& SetBlend(BlendState inBlend);
        ColorTargetState& SetWriteFlags(ColorWriteFlags inFlags);
    };

    struct FragmentState {
        std::vector<ColorTargetState> colorTargets;

        FragmentState();
        FragmentState& AddColorTarget(const ColorTargetState& inState);
    };

    struct ComputePipelineCreateInfo {
        PipelineLayout* layout;
        ShaderModule* computeShader;

        ComputePipelineCreateInfo();
        ComputePipelineCreateInfo& SetLayout(PipelineLayout* inLayout);
        ComputePipelineCreateInfo& SetComputeShader(ShaderModule* inComputeShader);
    };

    struct RasterPipelineCreateInfo {
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

        RasterPipelineCreateInfo();
        RasterPipelineCreateInfo& SetLayout(PipelineLayout* inLayout);
        RasterPipelineCreateInfo& SetVertexShader(ShaderModule* inVertexShader);
        RasterPipelineCreateInfo& SetPixelShader(ShaderModule* inPixelShader);
        RasterPipelineCreateInfo& SetGeometryShader(ShaderModule* inGeometryShader);
        RasterPipelineCreateInfo& SetDomainShader(ShaderModule* inDomainShader);
        RasterPipelineCreateInfo& SetHullShader(ShaderModule* inHullShader);
        RasterPipelineCreateInfo& SetVertexState(const VertexState& inVertexState);
        RasterPipelineCreateInfo& SetPrimitiveState(const PrimitiveState& inPrimitiveState);
        RasterPipelineCreateInfo& SetDepthStencilState(const DepthStencilState& inDepthStencilState);
        RasterPipelineCreateInfo& SetMultiSampleState(const MultiSampleState& inMultiSampleState);
        RasterPipelineCreateInfo& SetFragmentState(const FragmentState& inFragmentState);
        RasterPipelineCreateInfo& SetDebugName(std::string inDebugName);
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

    class RasterPipeline : public Pipeline {
    public:
        NonCopyable(RasterPipeline)
        ~RasterPipeline() override;

        void Destroy() override = 0;

    protected:
        explicit RasterPipeline(const RasterPipelineCreateInfo& createInfo);
    };
}
