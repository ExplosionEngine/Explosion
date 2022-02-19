//
// Created by johnk on 19/2/2022.
//

#ifndef EXPLOSION_RHI_PIPELINE_H
#define EXPLOSION_RHI_PIPELINE_H

#include <string>

#include <Common/Utility.h>
#include <RHI/Enum.h>

namespace RHI {
    class BindGroupLayout;
    class PipelineLayout;
    class ShaderModule;

    struct PipelineConstant {
        std::string key;
        double value;
    };

    struct ProgrammableStage {
        ShaderModule* shaderModule = nullptr;
        std::string entryPoint {};
        size_t constantNum = 0;
        const PipelineConstant* constants = nullptr;
    };

    struct PipelineCreateInfo {
        PipelineLayout* layout;
    };

    struct VertexAttribute {
        VertexFormat format;
        size_t offset;
        size_t location;
    };

    struct VertexBufferLayout {
        size_t stride;
        VertexStepMode stepMode;
        size_t attributeNum;
        const VertexAttribute* attributes;
    };

    struct VertexState {
        ProgrammableStage stage;
        size_t bufferLayoutNum = 0;
        const VertexBufferLayout* bufferLayouts = nullptr;
    };

    struct PrimitiveState {
        PrimitiveTopology topology = PrimitiveTopology::TRIANGLE_LIST;
        IndexFormat stripIndexFormat = IndexFormat::UINT16;
        FrontFace frontFace = FrontFace::CCW;
        CullMode cullMode = CullMode::NONE;
        // TODO WebGPU spec ?
        bool depthClip = false;
    };

    struct StencilFaceState {
        CompareFunction compare = CompareFunction::ALWAYS;
        StencilOp failOp = StencilOp::KEEP;
        StencilOp depthFailOp = StencilOp::KEEP;
        StencilOp passOp = StencilOp::KEEP;
    };

    struct DepthStencilState {
        PixelFormat format = PixelFormat::DEPTH_32_FLOAT;
        bool writeDepth = false;
        CompareFunction depthCompare = CompareFunction::ALWAYS;
        StencilFaceState stencilFront;
        StencilFaceState stencilBack;
        uint32_t stencilReadMask = 0xffffffff;
        uint32_t stencilWriteMask = 0xffffffff;
        uint32_t depthBias = 0;
        float depthBiasSlopeScale = 0.f;
        float depthBiasClamp = 0.f;
    };

    struct MultiSampleState {
        size_t count = 0;
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
        ProgrammableStage stage;
        size_t targetNum = 0;
        const ColorTargetState* targets = nullptr;
    };

    struct ComputePipelineCreateInfo : public PipelineCreateInfo {
    public:
        ComputePipelineCreateInfo() : PipelineCreateInfo() {}

        ProgrammableStage computeStage;
    };

    struct GraphicsPipelineCreateInfo : public PipelineCreateInfo {
    public:
        GraphicsPipelineCreateInfo() : PipelineCreateInfo() {}

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

        virtual BindGroupLayout* GetBindGroupLayout(size_t index) = 0;
        virtual void Destroy() = 0;

    protected:
        Pipeline();
    };

    class ComputePipeline : public Pipeline {
    public:
        NON_COPYABLE(ComputePipeline)
        ~ComputePipeline() override;

        BindGroupLayout* GetBindGroupLayout(size_t index) override = 0;
        void Destroy() override = 0;

    protected:
        explicit ComputePipeline(const ComputePipelineCreateInfo* createInfo);
    };

    class GraphicsPipeline : public Pipeline {
    public:
        NON_COPYABLE(GraphicsPipeline)
        ~GraphicsPipeline() override;

        BindGroupLayout* GetBindGroupLayout(size_t index) override = 0;
        void Destroy() override = 0;

    protected:
        explicit GraphicsPipeline(const GraphicsPipelineCreateInfo* createInfo);
    };
}

#endif//EXPLOSION_RHI_PIPELINE_H
