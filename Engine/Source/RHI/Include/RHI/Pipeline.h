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
        const char* semanticName;
        uint8_t semanticIndex;
    };

    struct VertexBufferLayout {
        size_t stride;
        VertexStepMode stepMode;
        uint32_t attributeNum;
        const VertexAttribute* attributes;
    };

    struct VertexState {
        uint32_t bufferLayoutNum = 0;
        const VertexBufferLayout* bufferLayouts = nullptr;
    };

    struct PrimitiveState {
        // TODO fill mode ?
        PrimitiveTopologyType topologyType = PrimitiveTopologyType::triangle;
        IndexFormat stripIndexFormat = IndexFormat::uint16;
        FrontFace frontFace = FrontFace::ccw;
        CullMode cullMode = CullMode::none;
        bool depthClip = false;
    };

    struct StencilFaceState {
        ComparisonFunc comparisonFunc = ComparisonFunc::always;
        StencilOp failOp = StencilOp::keep;
        StencilOp depthFailOp = StencilOp::keep;
        StencilOp passOp = StencilOp::keep;
    };

    struct DepthStencilState {
        bool depthEnable = false;
        bool stencilEnable = false;
        PixelFormat format = PixelFormat::max;
        ComparisonFunc depthComparisonFunc = ComparisonFunc::always;
        StencilFaceState stencilFront;
        StencilFaceState stencilBack;
        uint8_t stencilReadMask = 0xff;
        uint8_t stencilWriteMask = 0xff;
        int32_t depthBias = 0;
        float depthBiasSlopeScale = 0.f;
        float depthBiasClamp = 0.f;
    };

    struct MultiSampleState {
        uint8_t count = 1;
        uint32_t mask = 0xffffffff;
        bool alphaToCoverage = false;
    };

    struct BlendComponent {
        BlendOp op = BlendOp::opAdd;
        BlendFactor srcFactor = BlendFactor::one;
        BlendFactor dstFactor = BlendFactor::zero;
    };

    struct BlendState {
        BlendComponent color;
        BlendComponent alpha;
    };

    struct ColorTargetState {
        PixelFormat format = PixelFormat::bgra8Unorm;
        BlendState blend;
        ColorWriteFlags writeFlags = ColorWriteBits::red | ColorWriteBits::green | ColorWriteBits::blue | ColorWriteBits::alpha;
    };

    struct FragmentState {
        uint8_t colorTargetNum = 0;
        const ColorTargetState* colorTargets = nullptr;
    };

    struct ComputePipelineCreateInfo {

        PipelineLayout* layout;
        ShaderModule* computeShader;
    };

    struct GraphicsPipelineCreateInfo {
        PipelineLayout* layout = nullptr;

        ShaderModule* vertexShader = nullptr;
        ShaderModule* pixelShader = nullptr;
        ShaderModule* geometryShader = nullptr;
        ShaderModule* domainShader = nullptr;
        ShaderModule* hullShader = nullptr;

        VertexState vertexState;
        PrimitiveState primitiveState;
        DepthStencilState depthStencilState;
        MultiSampleState multiSampleState;
        FragmentState fragmentState;
    };

    class Pipeline {
    public:
        NON_COPYABLE(Pipeline)
        virtual ~Pipeline();

        virtual void Destroy() = 0;

    protected:
        Pipeline();
    };

    class ComputePipeline : public Pipeline {
    public:
        NON_COPYABLE(ComputePipeline)
        ~ComputePipeline() override;

        void Destroy() override = 0;

    protected:
        explicit ComputePipeline(const ComputePipelineCreateInfo& createInfo);
    };

    class GraphicsPipeline : public Pipeline {
    public:
        NON_COPYABLE(GraphicsPipeline)
        ~GraphicsPipeline() override;

        void Destroy() override = 0;

    protected:
        explicit GraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo);
    };
}
