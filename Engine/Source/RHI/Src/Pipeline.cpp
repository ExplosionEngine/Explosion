//
// Created by johnk on 19/2/2022.
//

#include <RHI/Pipeline.h>

namespace RHI {
    VertexAttribute::VertexAttribute()
        : format(VertexFormat::max)
        , offset(0)
        , semanticName()
        , semanticIndex(0)
    {
    }

    VertexAttribute& VertexAttribute::Format(VertexFormat inFormat)
    {
        format = inFormat;
        return *this;
    }

    VertexAttribute& VertexAttribute::Offset(size_t inOffset)
    {
        offset = inOffset;
        return *this;
    }

    VertexAttribute& VertexAttribute::SemanticName(std::string inSemanticName)
    {
        semanticName = std::move(inSemanticName);
        return *this;
    }

    VertexAttribute& VertexAttribute::SemanticIndex(uint8_t inSemanticIndex)
    {
        semanticIndex = inSemanticIndex;
        return *this;
    }

    VertexBufferLayout::VertexBufferLayout()
        : stride(0)
        , stepMode(VertexStepMode::max)
        , attributes()
    {
    }

    VertexBufferLayout& VertexBufferLayout::Stride(size_t inStride)
    {
        stride = inStride;
        return *this;
    }

    VertexBufferLayout& VertexBufferLayout::StepMode(VertexStepMode inStepMode)
    {
        stepMode = inStepMode;
        return *this;
    }

    VertexBufferLayout& VertexBufferLayout::Attribute(const VertexAttribute& inAttribute)
    {
        attributes.emplace_back(inAttribute);
        return *this;
    }

    VertexState::VertexState()
        : bufferLayouts()
    {
    }

    VertexState& VertexState::VertexBufferLayout(const struct VertexBufferLayout& inLayout)
    {
        bufferLayouts.emplace_back(inLayout);
        return *this;
    }

    PrimitiveState::PrimitiveState()
        : topologyType(PrimitiveTopologyType::triangle)
        , stripIndexFormat(IndexFormat::uint16)
        , frontFace(FrontFace::ccw)
        , cullMode(CullMode::none)
        , depthClip(false)
    {
    }

    PrimitiveState& PrimitiveState::TopologyType(PrimitiveTopologyType inTopologyType)
    {
        topologyType = inTopologyType;
        return *this;
    }

    PrimitiveState& PrimitiveState::StripIndexFormat(IndexFormat inFormat)
    {
        stripIndexFormat = inFormat;
        return *this;
    }

    PrimitiveState& PrimitiveState::FrontFace(enum FrontFace inFrontFace)
    {
        frontFace = inFrontFace;
        return *this;
    }

    PrimitiveState& PrimitiveState::CullMode(enum CullMode inCullMode)
    {
        cullMode = inCullMode;
        return *this;
    }

    PrimitiveState& PrimitiveState::DepthClip(bool inDepthClip)
    {
        depthClip = inDepthClip;
        return *this;
    }

    StencilFaceState::StencilFaceState()
        : comparisonFunc(ComparisonFunc::always)
        , failOp(StencilOp::keep)
        , depthFailOp(StencilOp::keep)
        , passOp(StencilOp::keep)
    {
    }

    StencilFaceState& StencilFaceState::ComparisonFunc(enum ComparisonFunc inFunc)
    {
        comparisonFunc = inFunc;
        return *this;
    }

    StencilFaceState& StencilFaceState::FailOp(StencilOp inFailOp)
    {
        failOp = inFailOp;
        return *this;
    }

    StencilFaceState& StencilFaceState::DepthFailOp(StencilOp inDepthFailOp)
    {
        depthFailOp = inDepthFailOp;
        return *this;
    }

    StencilFaceState& StencilFaceState::PassOp(StencilOp inPassOp)
    {
        passOp = inPassOp;
        return *this;
    }

    DepthStencilState::DepthStencilState()
        : depthEnable(false)
        , stencilEnable(false)
        , format(PixelFormat::max)
        , depthComparisonFunc(ComparisonFunc::always)
        , stencilFront()
        , stencilBack()
        , stencilReadMask()
        , stencilWriteMask()
        , depthBias(0)
        , depthBiasSlopeScale(0)
        , depthBiasClamp(0)
    {
    }

    DepthStencilState& DepthStencilState::DepthEnabled(bool inDepthEnabled)
    {
        depthEnable = inDepthEnabled;
        return *this;
    }

    DepthStencilState& DepthStencilState::StencilEnabled(bool inStencilEnabled)
    {
        stencilEnable = inStencilEnabled;
        return *this;
    }

    DepthStencilState& DepthStencilState::Format(PixelFormat inFormat)
    {
        format = inFormat;
        return *this;
    }

    DepthStencilState& DepthStencilState::DepthComparisonFunc(ComparisonFunc inFunc)
    {
        depthComparisonFunc = inFunc;
        return *this;
    }

    DepthStencilState& DepthStencilState::StencilFront(StencilFaceState inState)
    {
        stencilFront = inState;
        return *this;
    }

    DepthStencilState& DepthStencilState::StencilBack(StencilFaceState inState)
    {
        stencilBack = inState;
        return *this;
    }

    DepthStencilState& DepthStencilState::StencilReadMask(uint8_t inStencilReadMask)
    {
        stencilReadMask = inStencilReadMask;
        return *this;
    }

    DepthStencilState& DepthStencilState::StencilWriteMask(uint8_t inStencilWriteMask)
    {
        stencilWriteMask = inStencilWriteMask;
        return *this;
    }

    DepthStencilState& DepthStencilState::DepthBias(int32_t inDepthBias)
    {
        depthBias = inDepthBias;
        return *this;
    }

    DepthStencilState& DepthStencilState::DepthBiasSlopeScale(float inDepthBiasSlopeScale)
    {
        depthBiasSlopeScale = inDepthBiasSlopeScale;
        return *this;
    }

    DepthStencilState& DepthStencilState::DepthBiasClamp(float inDepthBiasClamp)
    {
        depthBiasClamp = inDepthBiasClamp;
        return *this;
    }

    MultiSampleState::MultiSampleState()
        : count(1)
        , mask(0xffffffff)
        , alphaToCoverage(false)
    {
    }

    MultiSampleState& MultiSampleState::Count(uint8_t inCount)
    {
        count = inCount;
        return *this;
    }

    MultiSampleState& MultiSampleState::Mask(uint32_t inMask)
    {
        mask = inMask;
        return *this;
    }

    MultiSampleState& MultiSampleState::AlphaToCoverage(bool inAlphaToCoverage)
    {
        alphaToCoverage = inAlphaToCoverage;
        return *this;
    }

    BlendComponent::BlendComponent()
        : op(BlendOp::opAdd)
        , srcFactor(BlendFactor::one)
        , dstFactor(BlendFactor::zero)
    {
    }

    BlendComponent& BlendComponent::Op(BlendOp inOp)
    {
        op = inOp;
        return *this;
    }

    BlendComponent& BlendComponent::SrcFactor(BlendFactor inSrcFactor)
    {
        srcFactor = inSrcFactor;
        return *this;
    }

    BlendComponent& BlendComponent::DstFactor(BlendFactor inDstFactor)
    {
        dstFactor = inDstFactor;
        return *this;
    }

    BlendState::BlendState()
        : color()
        , alpha()
    {
    }

    BlendState& BlendState::Color(BlendComponent inColor)
    {
        color = inColor;
        return *this;
    }

    BlendState& BlendState::Alpha(BlendComponent inAlpha)
    {
        alpha = inAlpha;
        return *this;
    }

    ColorTargetState::ColorTargetState()
        : format(PixelFormat::max)
        , blend()
        , writeFlags(ColorWriteBits::red | ColorWriteBits::green | ColorWriteBits::blue | ColorWriteBits::alpha)
    {
    }

    ColorTargetState& ColorTargetState::Format(PixelFormat inFormat)
    {
        format = inFormat;
        return *this;
    }

    ColorTargetState& ColorTargetState::Blend(BlendState inBlend)
    {
        blend = inBlend;
        return *this;
    }

    ColorTargetState& ColorTargetState::WriteFlags(ColorWriteFlags inFlags)
    {
        writeFlags = inFlags;
        return *this;
    }

    FragmentState::FragmentState()
        : colorTargets()
    {
    }

    FragmentState& FragmentState::ColorTarget(const ColorTargetState& inState)
    {
        colorTargets.emplace_back(inState);
        return *this;
    }

    ComputePipelineCreateInfo::ComputePipelineCreateInfo()
        : layout(nullptr)
        , computeShader(nullptr)
    {
    }

    ComputePipelineCreateInfo& ComputePipelineCreateInfo::Layout(PipelineLayout* inLayout)
    {
        layout = inLayout;
        return *this;
    }

    ComputePipelineCreateInfo& ComputePipelineCreateInfo::ComputeShader(ShaderModule* inComputeShader)
    {
        computeShader = inComputeShader;
        return *this;
    }

    GraphicsPipelineCreateInfo::GraphicsPipelineCreateInfo()
        : layout(nullptr)
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

    GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo::Layout(PipelineLayout* inLayout)
    {
        layout = inLayout;
        return *this;
    }

    GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo::VertexShader(ShaderModule* inVertexShader)
    {
        vertexShader = inVertexShader;
        return *this;
    }

    GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo::PixelShader(ShaderModule* inPixelShader)
    {
        pixelShader = inPixelShader;
        return *this;
    }

    GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo::GeometryShader(ShaderModule* inGeometryShader)
    {
        geometryShader = inGeometryShader;
        return *this;
    }

    GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo::DomainShader(ShaderModule* inDomainShader)
    {
        domainShader = inDomainShader;
        return *this;
    }

    GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo::HullShader(ShaderModule* inHullShader)
    {
        hullShader = inHullShader;
        return *this;
    }

    GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo::VertexState(const struct VertexState& inVertexState)
    {
        vertexState = inVertexState;
        return *this;
    }

    GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo::PrimitiveState(const struct PrimitiveState& inPrimitiveState)
    {
        primitiveState = inPrimitiveState;
        return *this;
    }

    GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo::DepthStencilState(const struct DepthStencilState& inDepthStencilState)
    {
        depthStencilState = inDepthStencilState;
        return *this;
    }

    GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo::MultiSampleState(const struct MultiSampleState& inMultiSampleState)
    {
        multiSampleState = inMultiSampleState;
        return *this;
    }

    GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo::FragmentState(const struct FragmentState& inFragmentState)
    {
        fragmentState = inFragmentState;
        return *this;
    }

    GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo::DebugName(std::string inDebugName)
    {
        debugName = std::move(inDebugName);
        return *this;
    }

    Pipeline::Pipeline() = default;

    Pipeline::~Pipeline() = default;

    ComputePipeline::ComputePipeline(const ComputePipelineCreateInfo& createInfo) {}

    ComputePipeline::~ComputePipeline() = default;

    GraphicsPipeline::GraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo) {}

    GraphicsPipeline::~GraphicsPipeline() = default;
}
