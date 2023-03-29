//
// Created by johnk on 2023/3/11.
//

#pragma once

#include <memory>
#include <unordered_map>

#include <RHI/RHI.h>
#include <Render/Shader.h>

namespace Render {
    class PipelineLayout;
    class ComputePipelineState;
    class RasterPipelineState;

    using SamplerDesc = RHI::SamplerCreateInfo;
    using BindGroupLayoutDesc = RHI::BindGroupLayoutCreateInfo;
    using PipelineConstantLayoutDesc = RHI::PipelineConstantLayout;
    using VertexState = RHI::VertexState;
    using PrimitiveState = RHI::PrimitiveState;
    using DepthStencilState = RHI::DepthStencilState;
    using MultiSampleState = RHI::MultiSampleState;
    using FragmentState = RHI::FragmentState;

    struct PipelineLayoutDesc {
        uint32_t bindGroupLayoutNum;
        const BindGroupLayoutDesc* bindGroupLayoutDescs;
        uint32_t pipelineConstantNum;
        const PipelineConstantLayoutDesc* pipelineConstantLayoutDescs;

        size_t Hash() const;
    };

    struct ComputePipelineShaderSet {
        ShaderInstance computeShader;
    };

    struct RasterPipelineShaderSet {
        ShaderInstance vertexShader;
        ShaderInstance pixelShader;
        ShaderInstance geometryShader;
        ShaderInstance domainShader;
        ShaderInstance hullShader;
    };

    struct ComputePipelineStateDesc {
        ComputePipelineShaderSet shaders;

        size_t Hash() const;
    };

    struct RasterPipelineStateDesc {
        RasterPipelineShaderSet shaders;
        VertexState vertexState;
        PrimitiveState primitiveState;
        DepthStencilState depthStencilState;
        MultiSampleState multiSampleState;
        FragmentState fragmentState;

        size_t Hash() const;
    };

    class Sampler {
    public:
        ~Sampler();

        RHI::Sampler* GetRHI() const;

    private:
        friend class SamplerCache;

        Sampler(RHI::Device& inDevice, const SamplerDesc& inDesc);

        RHI::UniqueRef<RHI::Sampler> rhiHandle;
    };

    class PipelineLayout {
    public:
        ~PipelineLayout();

        RHI::BindGroupLayout* GetRHIBindGroupLayout(uint32_t layoutIndex) const;
        RHI::PipelineLayout* GetRHI() const;
        size_t GetHash() const;

    private:
        friend class PipelineLayoutCache;

        PipelineLayout(RHI::Device& inDevice, const PipelineLayoutDesc& inDesc, size_t inHash);

        size_t hash;
        std::unordered_map<uint32_t, RHI::UniqueRef<RHI::BindGroupLayout>> rhiBindGroupLayouts;
        RHI::UniqueRef<RHI::PipelineLayout> rhiHandle;
    };

    class ComputePipelineState {
    public:
        ~ComputePipelineState();

        PipelineLayout* GetPipelineLayout() const;
        RHI::ComputePipeline* GetRHI() const;
        size_t GetHash() const;

    private:
        friend class PipelineCache;

        ComputePipelineState(RHI::Device& inDevice, const ComputePipelineStateDesc& inDesc, size_t inHash);

        size_t hash;
        PipelineLayout* pipelineLayout;
        RHI::UniqueRef<RHI::ComputePipeline> rhiHandle;
    };

    class RasterPipelineState {
    public:
        ~RasterPipelineState();

        PipelineLayout* GetPipelineLayout() const;
        RHI::GraphicsPipeline* GetRHI() const;
        size_t GetHash() const;

    private:
        friend class PipelineCache;

        RasterPipelineState(RHI::Device& inDevice, const RasterPipelineStateDesc& inDesc, size_t inHash);

        size_t hash;
        PipelineLayout* pipelineLayout;
        RHI::UniqueRef<RHI::GraphicsPipeline> rhiHandle;
    };

    class SamplerCache {
    public:
        static SamplerCache& Get(RHI::Device& device);
        ~SamplerCache();

        Sampler* FindOrCreateSampler(const SamplerDesc& desc);

    private:
        explicit SamplerCache(RHI::Device& inDevice);

        RHI::Device& device;
        std::unordered_map<size_t, std::unique_ptr<Sampler>> samplers;
    };

    class PipelineCache {
    public:
        static PipelineCache& Get(RHI::Device& device);
        ~PipelineCache();

        // TODO offline pipeline cache
        void Invalidate();
        ComputePipelineState* GetPipeline(const ComputePipelineStateDesc& desc);
        RasterPipelineState* GetPipeline(const RasterPipelineStateDesc& desc);

    private:
        explicit PipelineCache(RHI::Device& inDevice);

        RHI::Device& device;
        std::unordered_map<size_t, std::unique_ptr<ComputePipelineState>> computePipelines;
        std::unordered_map<size_t, std::unique_ptr<RasterPipelineState>> rasterPipelines;
    };
}
