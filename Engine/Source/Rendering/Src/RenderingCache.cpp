//
// Created by johnk on 2023/3/11.
//

#include <Rendering/RenderingCache.h>

#include <utility>

namespace Rendering {
    class PipelineLayoutCache {
    public:
        static PipelineLayoutCache& Get(RHI::Device& device);
        ~PipelineLayoutCache();

        void Invalidate();

        template <typename D>
        requires std::is_same_v<D, ComputePipelineLayoutDesc> || std::is_same_v<D, RasterPipelineLayoutDesc>
        PipelineLayout* GetLayout(const D& desc)
        {
            auto hash = desc.Hash();
            auto iter = pipelineLayouts.find(hash);
            if (iter == pipelineLayouts.end()) {
                pipelineLayouts[hash] = Common::UniqueRef<PipelineLayout>(new PipelineLayout(device, desc, hash));
            }
            return pipelineLayouts[hash].Get();
        }

    private:
        explicit PipelineLayoutCache(RHI::Device& inDevice);

        RHI::Device& device;
        std::unordered_map<size_t, Common::UniqueRef<PipelineLayout>> pipelineLayouts;
    };

    PipelineLayoutCache& PipelineLayoutCache::Get(RHI::Device& device)
    {
        static std::unordered_map<RHI::Device*, Common::UniqueRef<PipelineLayoutCache>> map;

        auto iter = map.find(&device);
        if (iter == map.end()) {
            map[&device] = Common::UniqueRef<PipelineLayoutCache>(new PipelineLayoutCache(device));
        }
        return *map[&device];
    }

    PipelineLayoutCache::PipelineLayoutCache(RHI::Device& inDevice)
        : device(inDevice)
    {
    }

    PipelineLayoutCache::~PipelineLayoutCache() = default;

    void PipelineLayoutCache::Invalidate()
    {
        pipelineLayouts.clear();
    }
}

namespace Rendering {
    VertexBinding::VertexBinding()
        : semanticName()
        , semanticIndex(0)
    {
    }

    VertexBinding::VertexBinding(std::string inSemanticName, uint8_t inSemanticIndex)
        : semanticName(std::move(inSemanticName))
        , semanticIndex(inSemanticIndex)
    {
    }

    std::string VertexBinding::FinalSemantic() const
    {
        if (semanticIndex == 0) {
            return semanticName;
        }
        return semanticName + std::to_string(semanticIndex);
    }

    RHI::PlatformVertexBinding VertexBinding::GetRHI(const Render::ShaderReflectionData& inReflectionData) const
    {
        return inReflectionData.QueryVertexBindingChecked(FinalSemantic());
    }

    size_t VertexBinding::Hash() const
    {
        return Common::HashUtils::CityHash(this, sizeof(VertexBinding));
    }

    VertexAttribute::VertexAttribute(const VertexBinding& inBinding, RHI::VertexFormat inFormat, size_t inOffset)
        : RHI::VertexAttributeBase<VertexAttribute>(inFormat, inOffset)
        , binding(inBinding)
    {
    }

    VertexAttribute& VertexAttribute::SetBinding(const VertexBinding& inBinding)
    {
        binding = inBinding;
        return *this;
    }

    size_t VertexAttribute::Hash() const
    {
        return Common::HashUtils::CityHash(this, sizeof(VertexAttribute));
    }

    RHI::VertexAttribute VertexAttribute::GetRHI(const Render::ShaderReflectionData& inReflectionData) const
    {
        return RHI::VertexAttribute(binding.GetRHI(inReflectionData), format, offset);
    }

    VertexBufferLayout::VertexBufferLayout(RHI::VertexStepMode inStepMode, size_t inStride)
        : RHI::VertexBufferLayoutBase<VertexBufferLayout>(inStepMode, inStride)
    {
    }

    RHI::VertexBufferLayout VertexBufferLayout::GetRHI(const Render::ShaderReflectionData& inReflectionData) const
    {
        RHI::VertexBufferLayout result(stepMode, stride);
        result.attributes.reserve(attributes.size());
        for (const auto& attribute : attributes) {
            result.AddAttribute(attribute.GetRHI(inReflectionData));
        }
        return result;
    }

    VertexBufferLayout& VertexBufferLayout::AddAttribute(const VertexAttribute& inAttribute)
    {
        attributes.emplace_back(inAttribute);
        return *this;
    }

    size_t VertexBufferLayout::Hash() const
    {
        std::vector<size_t> values;
        values.reserve(attributes.size());

        for (const auto& attribute : attributes) {
            values.emplace_back(attribute.Hash());
        }
        return Common::HashUtils::CityHash(values.data(), values.size() * sizeof(size_t));
    }

    VertexState::VertexState() = default;

    RHI::VertexState VertexState::GetRHI(const Render::ShaderReflectionData& inReflectionData) const
    {
        RHI::VertexState result;
        result.bufferLayouts.reserve(bufferLayouts.size());
        for (const auto& bufferLayout : bufferLayouts) {
            result.AddVertexBufferLayout(bufferLayout.GetRHI(inReflectionData));
        }
        return result;
    }

    VertexState& VertexState::AddVertexBufferLayout(const VertexBufferLayout& inLayout)
    {
        bufferLayouts.emplace_back(inLayout);
        return *this;
    }

    size_t VertexState::Hash() const
    {
        std::vector<size_t> values;
        values.reserve(bufferLayouts.size());

        for (const auto& layout : bufferLayouts) {
            values.emplace_back(layout.Hash());
        }
        return Common::HashUtils::CityHash(values.data(), values.size() * sizeof(size_t ));
    }

    size_t ComputePipelineShaderSet::Hash() const
    {
        return computeShader.Hash();
    }

    size_t RasterPipelineShaderSet::Hash() const
    {
        std::vector<size_t> values = {
            vertexShader.Hash(),
            pixelShader.Hash(),
            geometryShader.Hash(),
            domainShader.Hash(),
            hullShader.Hash()
        };
        return Common::HashUtils::CityHash(values.data(), values.size() * sizeof(size_t));
    }

    size_t ComputePipelineLayoutDesc::Hash() const
    {
        return shaders.Hash();
    }

    size_t RasterPipelineLayoutDesc::Hash() const
    {
        return shaders.Hash();
    }

    size_t ComputePipelineStateDesc::Hash() const
    {
        return shaders.Hash();
    }

    size_t RasterPipelineStateDesc::Hash() const
    {
        std::vector<size_t> values = {
            shaders.Hash(),
            vertexState.Hash(),
            primitiveState.Hash(),
            depthStencilState.Hash(),
            multiSampleState.Hash(),
            fragmentState.Hash()
        };
        return Common::HashUtils::CityHash(values.data(), values.size() * sizeof(size_t));
    }

    Sampler::Sampler(RHI::Device& inDevice, const SamplerDesc& inDesc)
    {
        rhiHandle = inDevice.CreateSampler(inDesc);
    }

    Sampler::~Sampler() = default;

    RHI::Sampler* Sampler::GetRHI() const
    {
        return rhiHandle.Get();
    }

    BindGroupLayout::BindGroupLayout(RHI::Device& inDevice, const BindGroupLayoutDesc& inDesc)
        : bindings(inDesc.binding)
    {


        RHI::BindGroupLayoutCreateInfo createInfo(inDesc.layoutIndex);
        createInfo.layoutIndex = inDesc.layoutIndex;
        createInfo.entries.reserve(bindings.size());
        for (const auto& binding : bindings) {
            createInfo.entries.emplace_back(binding.second.second, binding.second.first);
        }
        rhiHandle = inDevice.CreateBindGroupLayout(createInfo);
    }

    BindGroupLayout::~BindGroupLayout() = default;

    const RHI::ResourceBinding* BindGroupLayout::GetBinding(const std::string& name, RHI::ShaderStageBits shaderStage) const
    {
        auto iter = bindings.find(name);
        if (iter == bindings.end()) {
            return nullptr;
        }
        const auto& bindingPair = iter->second;
        return bindingPair.first & shaderStage ? &bindingPair.second : nullptr;
    }

    const RHI::ResourceBinding* BindGroupLayout::GetBinding(const std::string& name)
    {
        auto iter = bindings.find(name);
        if (iter == bindings.end()) {
            return nullptr;
        }
        return &iter->second.second;
    }

    RHI::BindGroupLayout* BindGroupLayout::GetRHI() const
    {
        return rhiHandle.Get();
    }

    PipelineLayout::PipelineLayout(RHI::Device& inDevice, const ComputePipelineLayoutDesc& inDesc, size_t inHash)
        : hash(inHash)
    {
        std::vector<ShaderInstancePack> shaderInstancePacks = {
            { RHI::ShaderStageBits::sCompute, &inDesc.shaders.computeShader }
        };
        CreateBindGroupLayout(inDevice, shaderInstancePacks);
        CreateRHIPipelineLayout(inDevice);
    }

    PipelineLayout::PipelineLayout(RHI::Device& inDevice, const RasterPipelineLayoutDesc& inDesc, size_t inHash)
        : hash(inHash)
    {
        std::vector<ShaderInstancePack> shaderInstancePacks = {
            { RHI::ShaderStageBits::sVertex, &inDesc.shaders.vertexShader },
            { RHI::ShaderStageBits::sPixel, &inDesc.shaders.pixelShader },
            { RHI::ShaderStageBits::sGeometry, &inDesc.shaders.geometryShader },
            { RHI::ShaderStageBits::sDomain, &inDesc.shaders.domainShader },
            { RHI::ShaderStageBits::sHull, &inDesc.shaders.hullShader }
        };
        CreateBindGroupLayout(inDevice, shaderInstancePacks);
        CreateRHIPipelineLayout(inDevice);
    }

    void PipelineLayout::CreateBindGroupLayout(RHI::Device& device, const std::vector<ShaderInstancePack>& shaderInstancePack)
    {
        std::unordered_map<uint8_t, BindingMap> layoutBindingMaps;
        for (const auto& pack : shaderInstancePack) {
            Assert(pack.instance->reflectionData != nullptr);

            const auto& resourceBindings = pack.instance->reflectionData->resourceBindings;
            for (const auto& resourceBinding : resourceBindings) {
                auto layoutIndex = resourceBinding.second.first;
                const auto& name = resourceBinding.first;
                const auto& binding = resourceBinding.second.second;

                if (!layoutBindingMaps.contains(layoutIndex)) {
                    layoutBindingMaps[layoutIndex] = {};
                }
                auto& layoutBindingMap = layoutBindingMaps[layoutIndex];
                layoutBindingMap.emplace(std::make_pair(name, std::make_pair(pack.stage, binding)));
            }
        }

        for (const auto& iter : layoutBindingMaps) {
            BindGroupLayoutDesc desc;
            desc.layoutIndex = iter.first;
            desc.binding = iter.second;
            bindGroupLayouts[iter.first] = Common::UniqueRef<BindGroupLayout>(new BindGroupLayout(device, desc));
        }
    }

    void PipelineLayout::CreateRHIPipelineLayout(RHI::Device& device)
    {
        std::vector<const RHI::BindGroupLayout*> tBindGroupLayouts;
        tBindGroupLayouts.reserve(bindGroupLayouts.size());
        for (const auto& iter : bindGroupLayouts) {
            tBindGroupLayouts.emplace_back(iter.second.Get()->GetRHI());
        }

        RHI::PipelineLayoutCreateInfo createInfo;
        createInfo.bindGroupLayouts = tBindGroupLayouts;
        rhiHandle = device.CreatePipelineLayout(createInfo);
    }

    PipelineLayout::~PipelineLayout() = default;

    BindGroupLayout* PipelineLayout::GetBindGroupLayout(uint8_t layoutIndex) const
    {
        auto iter = bindGroupLayouts.find(layoutIndex);
        return iter == bindGroupLayouts.end() ? nullptr : iter->second.Get();
    }

    RHI::PipelineLayout* PipelineLayout::GetRHI() const
    {
        return rhiHandle.Get();
    }

    size_t PipelineLayout::GetHash() const
    {
        return hash;
    }

    ComputePipelineState::ComputePipelineState(RHI::Device& inDevice, const ComputePipelineStateDesc& inDesc, size_t inHash)
        : hash(inHash)
    {
        ComputePipelineLayoutDesc desc = { inDesc.shaders };
        pipelineLayout = PipelineLayoutCache::Get(inDevice).GetLayout(desc);

        RHI::ComputePipelineCreateInfo createInfo;
        createInfo.layout = pipelineLayout->GetRHI();
        createInfo.computeShader = inDesc.shaders.computeShader.rhiHandle;
        rhiHandle = inDevice.CreateComputePipeline(createInfo);
    }

    ComputePipelineState::~ComputePipelineState() = default;

    BindGroupLayout* ComputePipelineState::GetBindGroupLayout(uint8_t layoutIndex) const
    {
        return pipelineLayout->GetBindGroupLayout(layoutIndex);
    }

    PipelineLayout* ComputePipelineState::GetPipelineLayout() const
    {
        return pipelineLayout;
    }

    RHI::ComputePipeline* ComputePipelineState::GetRHI() const
    {
        return rhiHandle.Get();
    }

    size_t ComputePipelineState::GetHash() const
    {
        return hash;
    }

    RasterPipelineState::RasterPipelineState(RHI::Device& inDevice, const RasterPipelineStateDesc& inDesc, size_t inHash)
        : hash(inHash)
    {
        RasterPipelineLayoutDesc desc = { inDesc.shaders };
        pipelineLayout = PipelineLayoutCache::Get(inDevice).GetLayout(desc);

        Assert(inDesc.shaders.vertexShader.reflectionData);

        RHI::RasterPipelineCreateInfo createInfo;
        createInfo.layout = pipelineLayout->GetRHI();
        createInfo.vertexShader = inDesc.shaders.vertexShader.rhiHandle;
        createInfo.pixelShader = inDesc.shaders.pixelShader.rhiHandle;
        createInfo.geometryShader = inDesc.shaders.geometryShader.rhiHandle;
        createInfo.domainShader = inDesc.shaders.domainShader.rhiHandle;
        createInfo.hullShader = inDesc.shaders.hullShader.rhiHandle;
        createInfo.vertexState = inDesc.vertexState.GetRHI(*inDesc.shaders.vertexShader.reflectionData);
        createInfo.primitiveState = inDesc.primitiveState;
        createInfo.depthStencilState = inDesc.depthStencilState;
        createInfo.multiSampleState = inDesc.multiSampleState;
        createInfo.fragmentState = inDesc.fragmentState;
        rhiHandle = inDevice.CreateRasterPipeline(createInfo);
    }

    RasterPipelineState::~RasterPipelineState() = default;

    PipelineLayout* RasterPipelineState::GetPipelineLayout() const
    {
        return pipelineLayout;
    }

    RHI::RasterPipeline* RasterPipelineState::GetRHI() const
    {
        return rhiHandle.Get();
    }

    size_t RasterPipelineState::GetHash() const
    {
        return hash;
    }

    SamplerCache& SamplerCache::Get(RHI::Device& device)
    {
        static std::unordered_map<RHI::Device*, Common::UniqueRef<SamplerCache>> map;

        auto iter = map.find(&device);
        if (iter == map.end()) {
            map[&device] = Common::UniqueRef<SamplerCache>(new SamplerCache(device));
        }
        return *map[&device];
    }

    SamplerCache::SamplerCache(RHI::Device& inDevice)
        : device(inDevice)
    {
    }

    SamplerCache::~SamplerCache() = default;

    Sampler* SamplerCache::GetOrCreate(const SamplerDesc& desc)
    {
        size_t hash = Common::HashUtils::CityHash(&desc, sizeof(SamplerDesc));
        auto iter = samplers.find(hash);
        if (iter == samplers.end()) {
            samplers[hash] = Common::UniqueRef<Sampler>(new Sampler(device, desc));
        }
        return samplers[hash].Get();
    }

    PipelineCache& PipelineCache::Get(RHI::Device& device)
    {
        static std::unordered_map<RHI::Device*, Common::UniqueRef<PipelineCache>> map;

        auto iter = map.find(&device);
        if (iter == map.end()) {
            map[&device] = Common::UniqueRef<PipelineCache>(new PipelineCache(device));
        }
        return *map[&device];
    }

    PipelineCache::PipelineCache(RHI::Device& inDevice)
        : device(inDevice)
    {
    }

    PipelineCache::~PipelineCache() = default;

    void PipelineCache::Invalidate()
    {
        computePipelines.clear();
        rasterPipelines.clear();
    }

    ComputePipelineState* PipelineCache::GetOrCreate(const ComputePipelineStateDesc& desc)
    {
        auto hash = desc.Hash();
        auto iter = computePipelines.find(hash);
        if (iter == computePipelines.end()) {
            computePipelines[hash] = Common::UniqueRef<ComputePipelineState>(new ComputePipelineState(device, desc, hash));
        }
        return computePipelines[hash].Get();
    }

    RasterPipelineState* PipelineCache::GetOrCreate(const RasterPipelineStateDesc& desc)
    {
        auto hash = desc.Hash();
        auto iter = rasterPipelines.find(hash);
        if (iter == rasterPipelines.end()) {
            rasterPipelines[hash] = Common::UniqueRef<RasterPipelineState>(new RasterPipelineState(device, desc, hash));
        }
        return rasterPipelines[hash].Get();
    }

    ResourceViewCache& ResourceViewCache::Get(RHI::Device& device)
    {
        static std::unordered_map<RHI::Device*, Common::UniqueRef<ResourceViewCache>> map;

        auto iter = map.find(&device);
        if (iter == map.end()) {
            map.emplace(std::make_pair(&device, Common::UniqueRef<ResourceViewCache>(new ResourceViewCache(device))));
        }
        return *iter->second;
    }

    ResourceViewCache::ResourceViewCache(RHI::Device& inDevice)
        : device(inDevice)
    {
    }

    ResourceViewCache::~ResourceViewCache() = default;

    void ResourceViewCache::Invalidate()
    {
        bufferViews.clear();
        textureViews.clear();
    }

    void ResourceViewCache::Invalidate(RHI::Buffer* buffer)
    {
        auto iter = bufferViews.find(buffer);
        if (iter != bufferViews.end()) {
            iter->second.clear();
        }
    }

    void ResourceViewCache::Invalidate(RHI::Texture* texture)
    {
        auto iter = textureViews.find(texture);
        if (iter != textureViews.end()) {
            iter->second.clear();
        }
    }

    RHI::BufferView* ResourceViewCache::GetOrCreate(RHI::Buffer* buffer, const RHI::BufferViewCreateInfo& inDesc)
    {
        auto& views = bufferViews[buffer];

        auto hash = inDesc.Hash();
        auto iter = views.find(hash);
        if (iter == views.end()) {
            views.emplace(std::make_pair(hash, Common::UniqueRef<RHI::BufferView>(buffer->CreateBufferView(inDesc))));
        }
        return views.at(hash).Get();
    }

    RHI::TextureView* ResourceViewCache::GetOrCreate(RHI::Texture* texture, const RHI::TextureViewCreateInfo& inDesc)
    {
        auto& views = textureViews[texture];

        auto hash = inDesc.Hash();
        auto iter = views.find(hash);
        if (iter == views.end()) {
            views.emplace(std::make_pair(hash, Common::UniqueRef<RHI::TextureView>(texture->CreateTextureView(inDesc))));
        }
        return views.at(hash).Get();
    }
}
