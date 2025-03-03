//
// Created by johnk on 19/2/2022.
//

#include <RHI/Pipeline.h>
#include <Common/Hash.h>

namespace RHI {
    HlslVertexBinding::HlslVertexBinding()
        : semanticIndex(0)
    {
    }

    HlslVertexBinding::HlslVertexBinding(std::string inSemanticName, const uint8_t inSemanticIndex)
        : semanticName(std::move(inSemanticName))
        , semanticIndex(inSemanticIndex)
    {
    }

    GlslVertexBinding::GlslVertexBinding()
        : location(0)
    {
    }

    GlslVertexBinding::GlslVertexBinding(const uint8_t inLocation)
        : location(inLocation)
    {
    }

    VertexAttribute::VertexAttribute(const PlatformVertexBinding& inPlatformBinding, const VertexFormat inFormat, const size_t inOffset)
        : VertexAttributeBase(inFormat, inOffset)
        , platformBinding(inPlatformBinding)
    {
    }

    VertexAttribute& VertexAttribute::SetPlatformBinding(const PlatformVertexBinding& inPlatformBinding)
    {
        platformBinding = inPlatformBinding;
        return *this;
    }

    VertexBufferLayout::VertexBufferLayout(const VertexStepMode inStepMode, const size_t inStride)
        : VertexBufferLayoutBase(inStepMode, inStride)
    {
    }

    VertexBufferLayout& VertexBufferLayout::AddAttribute(const VertexAttribute& inAttribute)
    {
        attributes.emplace_back(inAttribute);
        return *this;
    }

    VertexState::VertexState()
    {
    }

    VertexState& VertexState::AddVertexBufferLayout(const VertexBufferLayout& inLayout)
    {
        bufferLayouts.emplace_back(inLayout);
        return *this;
    }

    PrimitiveState::PrimitiveState(
        const PrimitiveTopologyType inTopologyType,
        const FillMode inFillMode,
        const IndexFormat inStripIndexFormat,
        const FrontFace inFrontFace,
        const CullMode inCullMode,
        const bool inDepthClip)
        : topologyType(inTopologyType)
        , fillMode(inFillMode)
        , stripIndexFormat(inStripIndexFormat)
        , frontFace(inFrontFace)
        , cullMode(inCullMode)
        , depthClip(inDepthClip)
    {
    }

    PrimitiveState& PrimitiveState::SetTopologyType(const PrimitiveTopologyType inTopologyType)
    {
        topologyType = inTopologyType;
        return *this;
    }

    PrimitiveState& PrimitiveState::SetFillMode(const FillMode inFillMode)
    {
        fillMode = inFillMode;
        return *this;
    }

    PrimitiveState& PrimitiveState::SetStripIndexFormat(const IndexFormat inFormat)
    {
        stripIndexFormat = inFormat;
        return *this;
    }

    PrimitiveState& PrimitiveState::SetFrontFace(const FrontFace inFrontFace)
    {
        frontFace = inFrontFace;
        return *this;
    }

    PrimitiveState& PrimitiveState::SetCullMode(const CullMode inCullMode)
    {
        cullMode = inCullMode;
        return *this;
    }

    PrimitiveState& PrimitiveState::SetDepthClip(const bool inDepthClip)
    {
        depthClip = inDepthClip;
        return *this;
    }

    uint64_t PrimitiveState::Hash() const
    {
        return Common::HashUtils::CityHash(this, sizeof(PrimitiveState));
    }

    StencilFaceState::StencilFaceState(
        const CompareFunc inCompareFunc,
        const StencilOp inFailOp,
        const StencilOp inDepthFailOp,
        const StencilOp inPassOp)
        : compareFunc(inCompareFunc)
        , failOp(inFailOp)
        , depthFailOp(inDepthFailOp)
        , passOp(inPassOp)
    {
    }

    DepthStencilState::DepthStencilState(
        const bool inDepthEnabled,
        const bool inStencilEnabled,
        const PixelFormat inFormat,
        const CompareFunc inDepthCompareFunc,
        const int32_t inDepthBias,
        const float inDepthBiasSlopeScale,
        const float inDepthBiasClamp,
        const StencilFaceState& inStencilFront,
        const StencilFaceState& inStencilBack,
        const uint8_t inStencilReadMask,
        const uint8_t inStencilWriteMask)
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

    DepthStencilState& DepthStencilState::SetDepthEnabled(const bool inDepthEnabled)
    {
        depthEnabled = inDepthEnabled;
        return *this;
    }

    DepthStencilState& DepthStencilState::SetStencilEnabled(const bool inStencilEnabled)
    {
        stencilEnabled = inStencilEnabled;
        return *this;
    }

    DepthStencilState& DepthStencilState::SetFormat(const PixelFormat inFormat)
    {
        format = inFormat;
        return *this;
    }

    DepthStencilState& DepthStencilState::SetDepthCompareFunc(const CompareFunc inFunc)
    {
        depthCompareFunc = inFunc;
        return *this;
    }

    DepthStencilState& DepthStencilState::SetStencilFront(const StencilFaceState inState)
    {
        stencilFront = inState;
        return *this;
    }

    DepthStencilState& DepthStencilState::SetStencilBack(const StencilFaceState inState)
    {
        stencilBack = inState;
        return *this;
    }

    DepthStencilState& DepthStencilState::SetStencilReadMask(const uint8_t inStencilReadMask)
    {
        stencilReadMask = inStencilReadMask;
        return *this;
    }

    DepthStencilState& DepthStencilState::SetStencilWriteMask(const uint8_t inStencilWriteMask)
    {
        stencilWriteMask = inStencilWriteMask;
        return *this;
    }

    DepthStencilState& DepthStencilState::SetDepthBias(const int32_t inDepthBias)
    {
        depthBias = inDepthBias;
        return *this;
    }

    DepthStencilState& DepthStencilState::SetDepthBiasSlopeScale(const float inDepthBiasSlopeScale)
    {
        depthBiasSlopeScale = inDepthBiasSlopeScale;
        return *this;
    }

    DepthStencilState& DepthStencilState::SetDepthBiasClamp(const float inDepthBiasClamp)
    {
        depthBiasClamp = inDepthBiasClamp;
        return *this;
    }

    uint64_t DepthStencilState::Hash() const
    {
        return Common::HashUtils::CityHash(this, sizeof(DepthStencilState));
    }

    MultiSampleState::MultiSampleState(const uint8_t inCount, const uint32_t inMask, const bool inAlphaToCoverage)
        : count(inCount)
        , mask(inMask)
        , alphaToCoverage(inAlphaToCoverage)
    {
    }

    MultiSampleState& MultiSampleState::SetCount(const uint8_t inCount)
    {
        count = inCount;
        return *this;
    }

    MultiSampleState& MultiSampleState::SetMask(const uint32_t inMask)
    {
        mask = inMask;
        return *this;
    }

    MultiSampleState& MultiSampleState::SetAlphaToCoverage(const bool inAlphaToCoverage)
    {
        alphaToCoverage = inAlphaToCoverage;
        return *this;
    }

    uint64_t MultiSampleState::Hash() const
    {
        return Common::HashUtils::CityHash(this, sizeof(MultiSampleState));
    }

    BlendComponent::BlendComponent(const BlendOp inOp, const BlendFactor inSrcFactor, const BlendFactor inDstFactor)
        : op(inOp)
        , srcFactor(inSrcFactor)
        , dstFactor(inDstFactor)
    {
    }

    ColorTargetState::ColorTargetState(
        const PixelFormat inFormat,
        const ColorWriteFlags inWriteFlags,
        const bool inBlendEnabled,
        const BlendComponent& inColorBlend,
        const BlendComponent& inAlphaBlend)
        : format(inFormat)
        , writeFlags(inWriteFlags)
        , blendEnabled(inBlendEnabled)
        , colorBlend(inColorBlend)
        , alphaBlend(inAlphaBlend)
    {
    }

    ColorTargetState& ColorTargetState::SetFormat(const PixelFormat inFormat)
    {
        format = inFormat;
        return *this;
    }

    ColorTargetState& ColorTargetState::SetWriteFlags(const ColorWriteFlags inFlags)
    {
        writeFlags = inFlags;
        return *this;
    }

    ColorTargetState& ColorTargetState::SetBlendEnabled(const bool inBlendEnabled)
    {
        blendEnabled = inBlendEnabled;
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

    uint64_t ColorTargetState::Hash() const
    {
        return Common::HashUtils::CityHash(this, sizeof(ColorTargetState));
    }

    FragmentState::FragmentState()
    {
    }

    FragmentState& FragmentState::AddColorTarget(const ColorTargetState& inState)
    {
        colorTargets.emplace_back(inState);
        return *this;
    }

    uint64_t FragmentState::Hash() const
    {
        std::vector<uint64_t> values;
        values.reserve(colorTargets.size());

        for (const auto& colorTarget : colorTargets) {
            values.emplace_back(colorTarget.Hash());
        }
        return Common::HashUtils::CityHash(values.data(), values.size() * sizeof(uint64_t));
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

    ComputePipeline::ComputePipeline(const ComputePipelineCreateInfo&) {}

    ComputePipeline::~ComputePipeline() = default;

    RasterPipeline::RasterPipeline(const RasterPipelineCreateInfo&) {}

    RasterPipeline::~RasterPipeline() = default;
}
