//
// Created by johnk on 19/2/2022.
//

#include <RHI/Pipeline.h>

namespace RHI {
    VertexAttribute::VertexAttribute(
        std::string inSemanticName, uint8_t inSemanticIndex, VertexFormat inFormat, size_t inOffset)
        : semanticName(std::move(inSemanticName))
        , semanticIndex(inSemanticIndex)
        , format(inFormat)
        , offset(inOffset)
    {

    }

    VertexAttribute& VertexAttribute::SetSemanticName(std::string inSemanticName)
    {
        semanticName = std::move(inSemanticName);
        return *this;
    }

    VertexAttribute& VertexAttribute::SetSemanticIndex(uint8_t inSemanticIndex)
    {
        semanticIndex = inSemanticIndex;
        return *this;
    }

    VertexAttribute& VertexAttribute::SetFormat(VertexFormat inFormat)
    {
        format = inFormat;
        return *this;
    }

    VertexAttribute& VertexAttribute::SetOffset(size_t inOffset)
    {
        offset = inOffset;
        return *this;
    }

    VertexBufferLayout::VertexBufferLayout(VertexStepMode inStepMode, size_t inStride)
        : stepMode(inStepMode)
        , stride(inStride)
        , attributes()
    {
    }

    VertexBufferLayout& VertexBufferLayout::SetStride(size_t inStride)
    {
        stride = inStride;
        return *this;
    }

    VertexBufferLayout& VertexBufferLayout::SetStepMode(VertexStepMode inStepMode)
    {
        stepMode = inStepMode;
        return *this;
    }

    VertexBufferLayout& VertexBufferLayout::AddAttribute(const VertexAttribute& inAttribute)
    {
        attributes.emplace_back(inAttribute);
        return *this;
    }

    VertexState::VertexState()
        : bufferLayouts()
    {
    }

    VertexState& VertexState::AddVertexBufferLayout(const VertexBufferLayout& inLayout)
    {
        bufferLayouts.emplace_back(inLayout);
        return *this;
    }

    PrimitiveState::PrimitiveState(
        PrimitiveTopologyType inTopologyType, IndexFormat inStripIndexFormat, FrontFace inFrontFace, CullMode inCullMode, bool inDepthClip)
        : topologyType(inTopologyType)
        , stripIndexFormat(inStripIndexFormat)
        , frontFace(inFrontFace)
        , cullMode(inCullMode)
        , depthClip(inDepthClip)
    {
    }

    PrimitiveState& PrimitiveState::SetTopologyType(PrimitiveTopologyType inTopologyType)
    {
        topologyType = inTopologyType;
        return *this;
    }

    PrimitiveState& PrimitiveState::SetStripIndexFormat(IndexFormat inFormat)
    {
        stripIndexFormat = inFormat;
        return *this;
    }

    PrimitiveState& PrimitiveState::SetFrontFace(enum FrontFace inFrontFace)
    {
        frontFace = inFrontFace;
        return *this;
    }

    PrimitiveState& PrimitiveState::SetCullMode(enum CullMode inCullMode)
    {
        cullMode = inCullMode;
        return *this;
    }

    PrimitiveState& PrimitiveState::SetDepthClip(bool inDepthClip)
    {
        depthClip = inDepthClip;
        return *this;
    }

    StencilFaceState::StencilFaceState(
        CompareFunc inCompareFunc, StencilOp inFailOp, StencilOp inDepthFailOp, StencilOp inPassOp)
        : compareFunc(inCompareFunc)
        , failOp(inFailOp)
        , depthFailOp(inDepthFailOp)
        , passOp(inPassOp)
    {
    }

    DepthStencilState::DepthStencilState(
        bool inDepthEnabled,
        bool inStencilEnabled,
        PixelFormat inFormat,
        CompareFunc inDepthCompareFunc,
        int32_t inDepthBias,
        float inDepthBiasSlopeScale,
        float inDepthBiasClamp,
        const StencilFaceState& inStencilFront,
        const StencilFaceState& inStencilBack,
        uint8_t inStencilReadMask,
        uint8_t inStencilWriteMask)
        : depthEnabled(inDepthEnabled)
        , stencilEnabled(inStencilEnabled)
        , format(inFormat)
        , depthCompareFunc(inDepthCompareFunc)
        , depthBias(inDepthBias)
        , depthBiasSlopeScale(inDepthBiasSlopeScale)
        , depthBiasClamp(inDepthBiasClamp)
        , stencilFront(inStencilFront)
        , stencilBack(inStencilBack)
        , stencilReadMask(inStencilReadMask)
        , stencilWriteMask(inStencilWriteMask)
    {
    }

    DepthStencilState& DepthStencilState::SetDepthEnabled(bool inDepthEnabled)
    {
        depthEnabled = inDepthEnabled;
        return *this;
    }

    DepthStencilState& DepthStencilState::SetStencilEnabled(bool inStencilEnabled)
    {
        stencilEnabled = inStencilEnabled;
        return *this;
    }

    DepthStencilState& DepthStencilState::SetFormat(PixelFormat inFormat)
    {
        format = inFormat;
        return *this;
    }

    DepthStencilState& DepthStencilState::SetDepthCompareFunc(CompareFunc inFunc)
    {
        depthCompareFunc = inFunc;
        return *this;
    }

    DepthStencilState& DepthStencilState::SetStencilFront(StencilFaceState inState)
    {
        stencilFront = inState;
        return *this;
    }

    DepthStencilState& DepthStencilState::SetStencilBack(StencilFaceState inState)
    {
        stencilBack = inState;
        return *this;
    }

    DepthStencilState& DepthStencilState::SetStencilReadMask(uint8_t inStencilReadMask)
    {
        stencilReadMask = inStencilReadMask;
        return *this;
    }

    DepthStencilState& DepthStencilState::SetStencilWriteMask(uint8_t inStencilWriteMask)
    {
        stencilWriteMask = inStencilWriteMask;
        return *this;
    }

    DepthStencilState& DepthStencilState::SetDepthBias(int32_t inDepthBias)
    {
        depthBias = inDepthBias;
        return *this;
    }

    DepthStencilState& DepthStencilState::SetDepthBiasSlopeScale(float inDepthBiasSlopeScale)
    {
        depthBiasSlopeScale = inDepthBiasSlopeScale;
        return *this;
    }

    DepthStencilState& DepthStencilState::SetDepthBiasClamp(float inDepthBiasClamp)
    {
        depthBiasClamp = inDepthBiasClamp;
        return *this;
    }

    MultiSampleState::MultiSampleState(uint8_t inCount, uint32_t inMask, bool inAlphaToCoverage)
        : count(inCount)
        , mask(inMask)
        , alphaToCoverage(inAlphaToCoverage)
    {
    }

    MultiSampleState& MultiSampleState::SetCount(uint8_t inCount)
    {
        count = inCount;
        return *this;
    }

    MultiSampleState& MultiSampleState::SetMask(uint32_t inMask)
    {
        mask = inMask;
        return *this;
    }

    MultiSampleState& MultiSampleState::SetAlphaToCoverage(bool inAlphaToCoverage)
    {
        alphaToCoverage = inAlphaToCoverage;
        return *this;
    }

    BlendComponent::BlendComponent(BlendOp inOp, BlendFactor inSrcFactor, BlendFactor inDstFactor)
        : op(inOp)
        , srcFactor(inSrcFactor)
        , dstFactor(inDstFactor)
    {
    }

    ColorTargetState::ColorTargetState(
        PixelFormat inFormat, ColorWriteFlags inWriteFlags, const BlendComponent& inColorBlend, const BlendComponent& inAlphaBlend)
        : format(inFormat)
        , writeFlags(inWriteFlags)
        , colorBlend(inColorBlend)
        , alphaBlend(inAlphaBlend)
    {
    }

    ColorTargetState& ColorTargetState::SetFormat(PixelFormat inFormat)
    {
        format = inFormat;
        return *this;
    }

    ColorTargetState& ColorTargetState::SetWriteFlags(ColorWriteFlags inFlags)
    {
        writeFlags = inFlags;
        return *this;
    }

    ColorTargetState& ColorTargetState::SetColorBlend(const BlendComponent& inColorBlend)
    {
        colorBlend = inColorBlend;
        return *this;
    }

    ColorTargetState& ColorTargetState::SetAlphaBlend(const BlendComponent& inAlphaBlend)
    {
        alphaBlend = inAlphaBlend;
        return *this;
    }

    FragmentState::FragmentState()
        : colorTargets()
    {
    }

    FragmentState& FragmentState::AddColorTarget(const ColorTargetState& inState)
    {
        colorTargets.emplace_back(inState);
        return *this;
    }

    ComputePipelineCreateInfo::ComputePipelineCreateInfo()
        : layout(nullptr)
        , computeShader(nullptr)
    {
    }

    ComputePipelineCreateInfo& ComputePipelineCreateInfo::SetLayout(PipelineLayout* inLayout)
    {
        layout = inLayout;
        return *this;
    }

    ComputePipelineCreateInfo& ComputePipelineCreateInfo::SetComputeShader(ShaderModule* inComputeShader)
    {
        computeShader = inComputeShader;
        return *this;
    }

    RasterPipelineCreateInfo::RasterPipelineCreateInfo(PipelineLayout* inLayout)
        : layout(inLayout)
        , vertexShader(nullptr)
        , pixelShader(nullptr)
        , geometryShader(nullptr)
        , domainShader(nullptr)
        , hullShader(nullptr)
        , vertexState()
        , primitiveState()
        , depthStencilState()
        , multiSampleState()
        , fragmentState()
        , debugName()
    {
    }

    RasterPipelineCreateInfo& RasterPipelineCreateInfo::SetLayout(PipelineLayout* inLayout)
    {
        layout = inLayout;
        return *this;
    }

    RasterPipelineCreateInfo& RasterPipelineCreateInfo::SetVertexShader(ShaderModule* inVertexShader)
    {
        vertexShader = inVertexShader;
        return *this;
    }

    RasterPipelineCreateInfo& RasterPipelineCreateInfo::SetPixelShader(ShaderModule* inPixelShader)
    {
        pixelShader = inPixelShader;
        return *this;
    }

    RasterPipelineCreateInfo& RasterPipelineCreateInfo::SetGeometryShader(ShaderModule* inGeometryShader)
    {
        geometryShader = inGeometryShader;
        return *this;
    }

    RasterPipelineCreateInfo& RasterPipelineCreateInfo::SetDomainShader(ShaderModule* inDomainShader)
    {
        domainShader = inDomainShader;
        return *this;
    }

    RasterPipelineCreateInfo& RasterPipelineCreateInfo::SetHullShader(ShaderModule* inHullShader)
    {
        hullShader = inHullShader;
        return *this;
    }

    RasterPipelineCreateInfo& RasterPipelineCreateInfo::SetVertexState(const VertexState& inVertexState)
    {
        vertexState = inVertexState;
        return *this;
    }

    RasterPipelineCreateInfo& RasterPipelineCreateInfo::SetPrimitiveState(const PrimitiveState& inPrimitiveState)
    {
        primitiveState = inPrimitiveState;
        return *this;
    }

    RasterPipelineCreateInfo& RasterPipelineCreateInfo::SetDepthStencilState(
        const DepthStencilState& inDepthStencilState)
    {
        depthStencilState = inDepthStencilState;
        return *this;
    }

    RasterPipelineCreateInfo& RasterPipelineCreateInfo::SetMultiSampleState(
        const MultiSampleState& inMultiSampleState)
    {
        multiSampleState = inMultiSampleState;
        return *this;
    }

    RasterPipelineCreateInfo& RasterPipelineCreateInfo::SetFragmentState(const FragmentState& inFragmentState)
    {
        fragmentState = inFragmentState;
        return *this;
    }

    RasterPipelineCreateInfo& RasterPipelineCreateInfo::SetDebugName(std::string inDebugName)
    {
        debugName = std::move(inDebugName);
        return *this;
    }

    Pipeline::Pipeline() = default;

    Pipeline::~Pipeline() = default;

    ComputePipeline::ComputePipeline(const ComputePipelineCreateInfo& createInfo) {}

    ComputePipeline::~ComputePipeline() = default;

    RasterPipeline::RasterPipeline(const RasterPipelineCreateInfo& createInfo) {}

    RasterPipeline::~RasterPipeline() = default;
}
