//
// Created by johnk on 19/2/2022.
//

#pragma once

#include <string>

#include <Common/Utility.h>
#include <RHI/Enum.h>

namespace RHI {
    class BindGroupLayout;
    class PipelineLayout;
    class ShaderModule;

    struct VertexAttribute {
        VertexFormat format;
        size_t offset;
        // for Vulkan
        uint8_t location;
        // for DirectX 12
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
        PrimitiveTopologyType topologyType = PrimitiveTopologyType::TRIANGLE;
        IndexFormat stripIndexFormat = IndexFormat::UINT16;
        FrontFace frontFace = FrontFace::CCW;
        CullMode cullMode = CullMode::NONE;
        bool depthClip = false;
    };

    struct StencilFaceState {
        ComparisonFunc comparisonFunc = ComparisonFunc::ALWAYS;
        StencilOp failOp = StencilOp::KEEP;
        StencilOp depthFailOp = StencilOp::KEEP;
        StencilOp passOp = StencilOp::KEEP;
    };

    struct DepthStencilState {
        bool depthEnable = false;
        bool stencilEnable = false;
        PixelFormat format = PixelFormat::D32_FLOAT;
        ComparisonFunc depthComparisonFunc = ComparisonFunc::ALWAYS;
        StencilFaceState stencilFront;
        StencilFaceState stencilBack;
        uint8_t stencilReadMask = 0xff;
        uint8_t stencilWriteMask = 0xff;
        int32_t depthBias = 0;
        float depthBiasSlopeScale = 0.f;
        float depthBiasClamp = 0.f;
    };

    struct MultiSampleState {
        uint8_t count = 0;
        uint32_t mask = 0xffffffff;
        bool alphaToCoverage = false;
    };

    struct BlendComponent {
        BlendOp op = BlendOp::OP_ADD;
        BlendFactor srcFactor = BlendFactor::ONE;
        BlendFactor dstFactor = BlendFactor::ZERO;
    };

    struct BlendState {
        BlendComponent color;
        BlendComponent alpha;
    };

    struct ColorTargetState {
        PixelFormat format = PixelFormat::BGRA8_UNORM;
        BlendState blend;
        ColorWriteFlags writeFlags = ColorWriteBits::RED | ColorWriteBits::GREEN | ColorWriteBits::BLUE | ColorWriteBits::ALPHA;
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
        ShaderModule* fragmentShader = nullptr;
        // TODO more shader?

        VertexState vertex;
        PrimitiveState primitive;
        DepthStencilState depthStencil;
        MultiSampleState multiSample;
        FragmentState fragment;
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
        explicit ComputePipeline(const ComputePipelineCreateInfo* createInfo);
    };

    class GraphicsPipeline : public Pipeline {
    public:
        NON_COPYABLE(GraphicsPipeline)
        ~GraphicsPipeline() override;

        void Destroy() override = 0;

    protected:
        explicit GraphicsPipeline(const GraphicsPipelineCreateInfo* createInfo);
    };
}
