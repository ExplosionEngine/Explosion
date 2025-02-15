//
// Created by johnk on 2023/3/11.
//

#pragma once

#include <unordered_map>

#include <RHI/RHI.h>
#include <Render/Shader.h>

namespace Render {
    class PipelineLayout;
    class ComputePipelineState;
    class RasterPipelineState;

    using RBindingMap = std::unordered_map<std::string, std::pair<RHI::ShaderStageFlags, RHI::ResourceBinding>>;
    using RSamplerDesc = RHI::SamplerCreateInfo;
    using RPrimitiveState = RHI::PrimitiveState;
    using RDepthStencilState = RHI::DepthStencilState;
    using RMultiSampleState = RHI::MultiSampleState;
    using RFragmentState = RHI::FragmentState;

    struct RVertexBinding {
        std::string semanticName;
        uint8_t semanticIndex;

        RVertexBinding();
        RVertexBinding(std::string inSemanticName, uint8_t inSemanticIndex);

        std::string FinalSemantic() const;
        RHI::PlatformVertexBinding GetRHI(const Render::ShaderReflectionData& inReflectionData) const;
        size_t Hash() const;
    };

    struct RVertexAttribute : RHI::VertexAttributeBase<RVertexAttribute> {
        RVertexBinding binding;

        explicit RVertexAttribute(
            const RVertexBinding& inBinding = RVertexBinding(),
            RHI::VertexFormat inFormat = RHI::VertexFormat::max,
            size_t inOffset = 0);

        RHI::VertexAttribute GetRHI(const Render::ShaderReflectionData& inReflectionData) const;
        RVertexAttribute& SetBinding(const RVertexBinding& inBinding);
        size_t Hash() const;
    };

    struct RVertexBufferLayout : RHI::VertexBufferLayoutBase<RVertexBufferLayout> {
        std::vector<RVertexAttribute> attributes;

        explicit RVertexBufferLayout(
            RHI::VertexStepMode inStepMode = RHI::VertexStepMode::perVertex,
            size_t inStride = 0);

        RHI::VertexBufferLayout GetRHI(const Render::ShaderReflectionData& inReflectionData) const;
        RVertexBufferLayout& AddAttribute(const RVertexAttribute& inAttribute);
        size_t Hash() const;
    };

    struct RVertexState {
        std::vector<RVertexBufferLayout> bufferLayouts;

        RVertexState();

        RHI::VertexState GetRHI(const Render::ShaderReflectionData& inReflectionData) const;
        RVertexState& AddVertexBufferLayout(const RVertexBufferLayout& inLayout);
        size_t Hash() const;
    };

    struct BindGroupLayoutDesc {
        uint8_t layoutIndex;
        RBindingMap binding;
    };

    struct ComputePipelineShaderSet {
        Render::ShaderInstance computeShader;

        size_t Hash() const;
    };

    struct RasterPipelineShaderSet {
        Render::ShaderInstance vertexShader;
        Render::ShaderInstance pixelShader;
        Render::ShaderInstance geometryShader;
        Render::ShaderInstance domainShader;
        Render::ShaderInstance hullShader;

        size_t Hash() const;
    };

    struct ComputePipelineLayoutDesc {
        ComputePipelineShaderSet shaders;

        size_t Hash() const;
    };

    struct RasterPipelineLayoutDesc {
        RasterPipelineShaderSet shaders;

        size_t Hash() const;
    };

    template <typename T> concept AnyPipelineLayoutDesc = std::is_same_v<T, ComputePipelineLayoutDesc> || std::is_same_v<T, RasterPipelineLayoutDesc>;

    struct ComputePipelineStateDesc {
        ComputePipelineShaderSet shaders;

        size_t Hash() const;
    };

    struct RasterPipelineStateDesc {
        RasterPipelineShaderSet shaders;
        RVertexState vertexState;
        RPrimitiveState primitiveState;
        RDepthStencilState depthStencilState;
        RMultiSampleState multiSampleState;
        RFragmentState fragmentState;

        RasterPipelineStateDesc();
        RasterPipelineStateDesc& SetVertexShader(const Render::ShaderInstance& inShader);
        RasterPipelineStateDesc& SetPixelShader(const Render::ShaderInstance& inShader);
        RasterPipelineStateDesc& SetGeometryShader(const Render::ShaderInstance& inShader);
        RasterPipelineStateDesc& SetDomainShader(const Render::ShaderInstance& inShader);
        RasterPipelineStateDesc& SetHullShader(const Render::ShaderInstance& inShader);
        RasterPipelineStateDesc& SetVertexState(const RVertexState& inVertexState);
        RasterPipelineStateDesc& SetPrimitiveState(const RPrimitiveState& inPrimitiveState);
        RasterPipelineStateDesc& SetDepthStencilState(const RDepthStencilState& inDepthStencilState);
        RasterPipelineStateDesc& SetMultiSampleState(const RMultiSampleState& inMultiSampleState);
        RasterPipelineStateDesc& SetFragmentState(const RFragmentState& inFragmentState);
        size_t Hash() const;
    };

    class Sampler {
    public:
        ~Sampler();

        RHI::Sampler* GetRHI() const;

    private:
        friend class SamplerCache;

        Sampler(RHI::Device& inDevice, const RSamplerDesc& inDesc);

        Common::UniquePtr<RHI::Sampler> rhiHandle;
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

        RBindingMap bindings;
        Common::UniquePtr<RHI::BindGroupLayout> rhiHandle;
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
        std::unordered_map<uint32_t, Common::UniquePtr<BindGroupLayout>> bindGroupLayouts;
        Common::UniquePtr<RHI::PipelineLayout> rhiHandle;
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
        Common::UniquePtr<RHI::ComputePipeline> rhiHandle;
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
        Common::UniquePtr<RHI::RasterPipeline> rhiHandle;
    };

    class SamplerCache {
    public:
        static SamplerCache& Get(RHI::Device& device);
        ~SamplerCache();

        Sampler* GetOrCreate(const RSamplerDesc& desc);

    private:
        explicit SamplerCache(RHI::Device& inDevice);

        RHI::Device& device;
        std::unordered_map<size_t, Common::UniquePtr<Sampler>> samplers;
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
        std::unordered_map<size_t, Common::UniquePtr<ComputePipelineState>> computePipelines;
        std::unordered_map<size_t, Common::UniquePtr<RasterPipelineState>> rasterPipelines;
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
        std::unordered_map<RHI::Buffer*, std::unordered_map<size_t, Common::UniquePtr<RHI::BufferView>>> bufferViews;
        std::unordered_map<RHI::Texture*, std::unordered_map<size_t, Common::UniquePtr<RHI::TextureView>>> textureViews;
    };
}
