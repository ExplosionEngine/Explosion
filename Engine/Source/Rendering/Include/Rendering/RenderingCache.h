//
// Created by johnk on 2023/3/11.
//

#pragma once

#include <unordered_map>

#include <RHI/RHI.h>
#include <Render/Shader.h>

namespace Rendering {
    class PipelineLayout;
    class ComputePipelineState;
    class RasterPipelineState;

    using BindingMap = std::unordered_map<std::string, std::pair<RHI::ShaderStageFlags, RHI::ResourceBinding>>;
    using SamplerDesc = RHI::SamplerCreateInfo;
    using VertexState = RHI::VertexState;
    using PrimitiveState = RHI::PrimitiveState;
    using DepthStencilState = RHI::DepthStencilState;
    using MultiSampleState = RHI::MultiSampleState;
    using FragmentState = RHI::FragmentState;

    struct BindGroupLayoutDesc {
        uint8_t layoutIndex;
        BindingMap binding;
    };

    struct ComputePipelineShaderSet {
        Render::ShaderInstance computeShader;

        // TODO check this
        size_t Hash() const;
    };

    struct RasterPipelineShaderSet {
        Render::ShaderInstance vertexShader;
        Render::ShaderInstance pixelShader;
        Render::ShaderInstance geometryShader;
        Render::ShaderInstance domainShader;
        Render::ShaderInstance hullShader;

        // TODO check this
        size_t Hash() const;
    };

    struct ComputePipelineLayoutDesc {
        ComputePipelineShaderSet shaders;

        // TODO check this
        size_t Hash() const;
    };

    struct RasterPipelineLayoutDesc {
        RasterPipelineShaderSet shaders;

        // TODO check this
        size_t Hash() const;
    };

    struct ComputePipelineStateDesc {
        ComputePipelineShaderSet shaders;

        // TODO check this
        size_t Hash() const;
    };

    struct RasterPipelineStateDesc {
        RasterPipelineShaderSet shaders;
        VertexState vertexState;
        PrimitiveState primitiveState;
        DepthStencilState depthStencilState;
        MultiSampleState multiSampleState;
        FragmentState fragmentState;

        // TODO check this
        size_t Hash() const;
    };

    class Sampler {
    public:
        ~Sampler();

        RHI::Sampler* GetRHI() const;

    private:
        friend class SamplerCache;

        Sampler(RHI::Device& inDevice, const SamplerDesc& inDesc);

        Common::UniqueRef<RHI::Sampler> rhiHandle;
    };

    class BindGroupLayout {
    public:
        ~BindGroupLayout();

        const RHI::ResourceBinding* GetBinding(const std::string& name);
        const RHI::ResourceBinding* GetBinding(const std::string& name, RHI::ShaderStageBits shaderStage) const;
        RHI::BindGroupLayout* GetRHI() const;

    private:
        friend class PipelineLayout;

        BindGroupLayout(RHI::Device& inDevice, const BindGroupLayoutDesc& inDesc);

        BindingMap bindings;
        Common::UniqueRef<RHI::BindGroupLayout> rhiHandle;
    };

    class PipelineLayout {
    public:
        ~PipelineLayout();

        BindGroupLayout* GetBindGroupLayout(uint8_t layoutIndex) const;
        RHI::PipelineLayout* GetRHI() const;
        size_t GetHash() const;

    private:
        struct ShaderInstancePack {
            RHI::ShaderStageBits stage;
            const Render::ShaderInstance* instance;
        };

        friend class PipelineLayoutCache;

        PipelineLayout(RHI::Device& inDevice, const ComputePipelineLayoutDesc& inDesc, size_t inHash);
        PipelineLayout(RHI::Device& inDevice, const RasterPipelineLayoutDesc& inDesc, size_t inHash);
        void CreateBindGroupLayout(RHI::Device& device, const std::vector<ShaderInstancePack>& shaderInstancePack);
        void CreateRHIPipelineLayout(RHI::Device& device);

        size_t hash;
        std::unordered_map<uint32_t, Common::UniqueRef<BindGroupLayout>> bindGroupLayouts;
        Common::UniqueRef<RHI::PipelineLayout> rhiHandle;
    };

    class ComputePipelineState {
    public:
        ~ComputePipelineState();

        BindGroupLayout* GetBindGroupLayout(uint8_t layoutIndex) const;
        PipelineLayout* GetPipelineLayout() const;
        RHI::ComputePipeline* GetRHI() const;
        size_t GetHash() const;

    private:
        friend class PipelineCache;

        ComputePipelineState(RHI::Device& inDevice, const ComputePipelineStateDesc& inDesc, size_t inHash);

        size_t hash;
        PipelineLayout* pipelineLayout;
        Common::UniqueRef<RHI::ComputePipeline> rhiHandle;
    };

    class RasterPipelineState {
    public:
        ~RasterPipelineState();

        PipelineLayout* GetPipelineLayout() const;
        RHI::RasterPipeline* GetRHI() const;
        size_t GetHash() const;

    private:
        friend class PipelineCache;

        RasterPipelineState(RHI::Device& inDevice, const RasterPipelineStateDesc& inDesc, size_t inHash);

        size_t hash;
        PipelineLayout* pipelineLayout;
        Common::UniqueRef<RHI::RasterPipeline> rhiHandle;
    };

    class SamplerCache {
    public:
        static SamplerCache& Get(RHI::Device& device);
        ~SamplerCache();

        Sampler* GetOrCreate(const SamplerDesc& desc);

    private:
        explicit SamplerCache(RHI::Device& inDevice);

        RHI::Device& device;
        std::unordered_map<size_t, Common::UniqueRef<Sampler>> samplers;
    };

    class PipelineCache {
    public:
        static PipelineCache& Get(RHI::Device& device);
        ~PipelineCache();

        // TODO offline pipeline cache
        void Invalidate();
        ComputePipelineState* GetOrCreate(const ComputePipelineStateDesc& desc);
        RasterPipelineState* GetOrCreate(const RasterPipelineStateDesc& desc);

    private:
        explicit PipelineCache(RHI::Device& inDevice);

        RHI::Device& device;
        std::unordered_map<size_t, Common::UniqueRef<ComputePipelineState>> computePipelines;
        std::unordered_map<size_t, Common::UniqueRef<RasterPipelineState>> rasterPipelines;
    };

    class ResourceViewCache {
    public:
        static ResourceViewCache& Get(RHI::Device& device);
        ~ResourceViewCache();

        void Invalidate();
        void Invalidate(RHI::Buffer* buffer);
        void Invalidate(RHI::Texture* texture);
        RHI::BufferView* GetOrCreate(RHI::Buffer* buffer, const RHI::BufferViewCreateInfo& inDesc);
        RHI::TextureView* GetOrCreate(RHI::Texture* texture, const RHI::TextureViewCreateInfo& inDesc);

    private:
        explicit ResourceViewCache(RHI::Device& inDevice);

        RHI::Device& device;
        std::unordered_map<RHI::Buffer*, std::unordered_map<size_t, Common::UniqueRef<RHI::BufferView>>> bufferViews;
        std::unordered_map<RHI::Texture*, std::unordered_map<size_t, Common::UniqueRef<RHI::TextureView>>> textureViews;
    };
}
