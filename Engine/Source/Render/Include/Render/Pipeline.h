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
    class ComputePipeline;
    class RasterPipeline;

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

    struct ComputePipelineDesc {
        PipelineLayout* layout;
        ShaderInstance computeShader;

        size_t Hash() const;
    };

    struct RasterPipelineDesc {
        PipelineLayout* layout;

        ShaderInstance vertexShader;
        ShaderInstance pixelShader;
        ShaderInstance geometryShader;
        ShaderInstance domainShader;
        ShaderInstance hullShader;

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

        std::unique_ptr<RHI::Sampler> rhiHandle;
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
        std::unordered_map<uint32_t, std::unique_ptr<RHI::BindGroupLayout>> rhiBindGroupLayouts;
        std::unique_ptr<RHI::PipelineLayout> rhiHandle;
    };

    class ComputePipeline {
    public:
        ~ComputePipeline();

        RHI::ComputePipeline* GetRHI() const;
        size_t GetHash() const;

    private:
        friend class PipelineCache;

        ComputePipeline(RHI::Device& inDevice, const ComputePipelineDesc& inDesc, size_t inHash);

        size_t hash;
        std::unique_ptr<RHI::ComputePipeline> rhiHandle;
    };

    class RasterPipeline {
    public:
        ~RasterPipeline();

        RHI::GraphicsPipeline* GetRHI() const;
        size_t GetHash() const;

    private:
        friend class PipelineCache;

        RasterPipeline(RHI::Device& inDevice, const RasterPipelineDesc& inDesc, size_t inHash);

        size_t hash;
        std::unique_ptr<RHI::GraphicsPipeline> rhiHandle;
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

    class PipelineLayoutCache {
    public:
        static PipelineLayoutCache& Get(RHI::Device& device);
        ~PipelineLayoutCache();

        void Invalidate();
        PipelineLayout* FindOrCreatePipelineLayout(const PipelineLayoutDesc& desc);

    private:
        explicit PipelineLayoutCache(RHI::Device& inDevice);

        RHI::Device& device;
        std::unordered_map<size_t, std::unique_ptr<PipelineLayout>> pipelineLayouts;
    };

    class PipelineCache {
    public:
        static PipelineCache& Get(RHI::Device& device);
        ~PipelineCache();

        // TODO offline pipeline cache
        void Invalidate();
        ComputePipeline* FindOrCreateComputePipeline(const ComputePipelineDesc& desc);
        RasterPipeline* FindOrCreateRasterPipeline(const RasterPipelineDesc& desc);

    private:
        explicit PipelineCache(RHI::Device& inDevice);

        RHI::Device& device;
        std::unordered_map<size_t, std::unique_ptr<ComputePipeline>> computePipelines;
        std::unordered_map<size_t, std::unique_ptr<RasterPipeline>> rasterPipelines;
    };
}
