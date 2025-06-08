//
// Created by johnk on 2023/3/11.
//

#include <Render/RenderCache.h>

#include <utility>

#include <Common/IO.h>
#include <Core/Thread.h>

namespace Render::Internal {
    constexpr uint64_t resourceViewCacheReleaseFrameLatency = 2;
    constexpr uint64_t bindGroupCacheReleaseFrameLatency = 2;
}

namespace Render {
    class PipelineLayoutCache {
    public:
        static PipelineLayoutCache& Get(RHI::Device& device);
        ~PipelineLayoutCache();

        void Invalidate();

        template <AnyPipelineLayoutDesc D>
        PipelineLayout* GetLayout(const D& desc)
        {
            auto hash = desc.Hash();
            auto iter = pipelineLayouts.find(hash);
            if (iter == pipelineLayouts.end()) {
                pipelineLayouts[hash] = Common::UniquePtr<PipelineLayout>(new PipelineLayout(device, desc, hash));
            }
            return pipelineLayouts[hash].Get();
        }

    private:
        explicit PipelineLayoutCache(RHI::Device& inDevice);

        RHI::Device& device;
        std::unordered_map<size_t, Common::UniquePtr<PipelineLayout>> pipelineLayouts;
    };

    PipelineLayoutCache& PipelineLayoutCache::Get(RHI::Device& device)
    {
        static std::unordered_map<RHI::Device*, Common::UniquePtr<PipelineLayoutCache>> map;

        if (const auto iter = map.find(&device);
            iter == map.end()) {
            map[&device] = Common::UniquePtr(new PipelineLayoutCache(device));
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

namespace Render {
    RVertexBinding::RVertexBinding()
        : semanticName()
        , semanticIndex(0)
    {
    }

    RVertexBinding::RVertexBinding(std::string inSemanticName, uint8_t inSemanticIndex)
        : semanticName(std::move(inSemanticName))
        , semanticIndex(inSemanticIndex)
    {
    }

    std::string RVertexBinding::FinalSemantic() const
    {
        if (semanticIndex == 0) {
            return semanticName;
        }
        return semanticName + std::to_string(semanticIndex);
    }

    RHI::PlatformVertexBinding RVertexBinding::GetRHI(const Render::ShaderReflectionData& inReflectionData) const
    {
        return inReflectionData.QueryVertexBindingChecked(FinalSemantic());
    }

    uint64_t RVertexBinding::Hash() const
    {
        // NOTICE: string can not use city hash for this ptr, cause can be change every time allocated
        const std::vector<uint64_t> values = {
            Common::HashUtils::CityHash(semanticName.data(), semanticName.size()),
            static_cast<uint64_t>(semanticIndex)
        };
        return Common::HashUtils::CityHash(values.data(), values.size() * sizeof(uint64_t));
    }

    RVertexAttribute::RVertexAttribute(const RVertexBinding& inBinding, RHI::VertexFormat inFormat, size_t inOffset)
        : RHI::VertexAttributeBase<RVertexAttribute>(inFormat, inOffset)
        , binding(inBinding)
    {
    }

    RVertexAttribute& RVertexAttribute::SetBinding(const RVertexBinding& inBinding)
    {
        binding = inBinding;
        return *this;
    }

    uint64_t RVertexAttribute::Hash() const
    {
        const std::vector<uint64_t> values = {
            binding.Hash(),
            static_cast<uint64_t>(format),
            offset
        };
        return Common::HashUtils::CityHash(values.data(), values.size() * sizeof(uint64_t));
    }

    RHI::VertexAttribute RVertexAttribute::GetRHI(const Render::ShaderReflectionData& inReflectionData) const
    {
        return RHI::VertexAttribute(binding.GetRHI(inReflectionData), format, offset);
    }

    RVertexBufferLayout::RVertexBufferLayout(RHI::VertexStepMode inStepMode, size_t inStride)
        : RHI::VertexBufferLayoutBase<RVertexBufferLayout>(inStepMode, inStride)
    {
    }

    RHI::VertexBufferLayout RVertexBufferLayout::GetRHI(const Render::ShaderReflectionData& inReflectionData) const
    {
        RHI::VertexBufferLayout result(stepMode, stride);
        result.attributes.reserve(attributes.size());
        for (const auto& attribute : attributes) {
            result.AddAttribute(attribute.GetRHI(inReflectionData));
        }
        return result;
    }

    RVertexBufferLayout& RVertexBufferLayout::AddAttribute(const RVertexAttribute& inAttribute)
    {
        attributes.emplace_back(inAttribute);
        return *this;
    }

    uint64_t RVertexBufferLayout::Hash() const
    {
        std::vector<uint64_t> values;
        values.reserve(attributes.size());

        for (const auto& attribute : attributes) {
            values.emplace_back(attribute.Hash());
        }
        return Common::HashUtils::CityHash(values.data(), values.size() * sizeof(uint64_t));
    }

    RVertexState::RVertexState() = default;

    RHI::VertexState RVertexState::GetRHI(const Render::ShaderReflectionData& inReflectionData) const
    {
        RHI::VertexState result;
        result.bufferLayouts.reserve(bufferLayouts.size());
        for (const auto& bufferLayout : bufferLayouts) {
            result.AddVertexBufferLayout(bufferLayout.GetRHI(inReflectionData));
        }
        return result;
    }

    RVertexState& RVertexState::AddVertexBufferLayout(const RVertexBufferLayout& inLayout)
    {
        bufferLayouts.emplace_back(inLayout);
        return *this;
    }

    uint64_t RVertexState::Hash() const
    {
        std::vector<uint64_t> values;
        values.reserve(bufferLayouts.size());

        for (const auto& layout : bufferLayouts) {
            values.emplace_back(layout.Hash());
        }
        return Common::HashUtils::CityHash(values.data(), values.size() * sizeof(uint64_t));
    }

    uint64_t ComputePipelineShaderSet::Hash() const
    {
        return computeShader.Hash();
    }

    uint64_t RasterPipelineShaderSet::Hash() const
    {
        const std::vector<uint64_t> values = {
            vertexShader.Hash(),
            pixelShader.Hash(),
            geometryShader.Hash(),
            domainShader.Hash(),
            hullShader.Hash()
        };
        return Common::HashUtils::CityHash(values.data(), values.size() * sizeof(uint64_t));
    }

    uint64_t ComputePipelineLayoutDesc::Hash() const
    {
        return shaders.Hash();
    }

    uint64_t RasterPipelineLayoutDesc::Hash() const
    {
        return shaders.Hash();
    }

    uint64_t ComputePipelineStateDesc::Hash() const
    {
        return shaders.Hash();
    }

    RasterPipelineStateDesc::RasterPipelineStateDesc() = default;

    RasterPipelineStateDesc& RasterPipelineStateDesc::SetVertexShader(const Render::ShaderInstance& inShader)
    {
        shaders.vertexShader = inShader;
        return *this;
    }

    RasterPipelineStateDesc& RasterPipelineStateDesc::SetPixelShader(const Render::ShaderInstance& inShader)
    {
        shaders.pixelShader = inShader;
        return *this;
    }

    RasterPipelineStateDesc& RasterPipelineStateDesc::SetGeometryShader(const Render::ShaderInstance& inShader)
    {
        shaders.geometryShader = inShader;
        return *this;
    }

    RasterPipelineStateDesc& RasterPipelineStateDesc::SetDomainShader(const Render::ShaderInstance& inShader)
    {
        shaders.domainShader = inShader;
        return *this;
    }

    RasterPipelineStateDesc& RasterPipelineStateDesc::SetHullShader(const Render::ShaderInstance& inShader)
    {
        shaders.hullShader = inShader;
        return *this;
    }

    RasterPipelineStateDesc& RasterPipelineStateDesc::SetVertexState(const RVertexState& inVertexState)
    {
        vertexState = inVertexState;
        return *this;
    }

    RasterPipelineStateDesc& RasterPipelineStateDesc::SetPrimitiveState(const RPrimitiveState& inPrimitiveState)
    {
        primitiveState = inPrimitiveState;
        return *this;
    }

    RasterPipelineStateDesc& RasterPipelineStateDesc::SetDepthStencilState(const RDepthStencilState& inDepthStencilState)
    {
        depthStencilState = inDepthStencilState;
        return *this;
    }

    RasterPipelineStateDesc& RasterPipelineStateDesc::SetMultiSampleState(const RMultiSampleState& inMultiSampleState)
    {
        multiSampleState = inMultiSampleState;
        return *this;
    }

    RasterPipelineStateDesc& RasterPipelineStateDesc::SetFragmentState(const RFragmentState& inFragmentState)
    {
        fragmentState = inFragmentState;
        return *this;
    }

    uint64_t RasterPipelineStateDesc::Hash() const
    {
        const std::vector<uint64_t> values = {
            shaders.Hash(),
            vertexState.Hash(),
            primitiveState.Hash(),
            depthStencilState.Hash(),
            multiSampleState.Hash(),
            fragmentState.Hash()
        };
        return Common::HashUtils::CityHash(values.data(), values.size() * sizeof(uint64_t));
    }

    Sampler::Sampler(RHI::Device& inDevice, const RSamplerDesc& inDesc)
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
        for (const auto& [bindingName, stageAndBinding] : bindings) {
            const auto& [stage, binding] = stageAndBinding;
            createInfo.entries.emplace_back(binding, stage);
        }
        rhiHandle = inDevice.CreateBindGroupLayout(createInfo);
    }

    BindGroupLayout::~BindGroupLayout() = default;

    const RHI::ResourceBinding* BindGroupLayout::GetBinding(const std::string& name, const RHI::ShaderStageBits shaderStage) const
    {
        const auto iter = bindings.find(name);
        if (iter == bindings.end()) {
            return nullptr;
        }
        const auto& [stage, binding] = iter->second;
        return stage & shaderStage ? &binding : nullptr;
    }

    const RHI::ResourceBinding* BindGroupLayout::GetBinding(const std::string& name)
    {
        const auto iter = bindings.find(name);
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
        const std::vector<ShaderInstancePack> shaderInstancePacks = {
            { RHI::ShaderStageBits::sCompute, &inDesc.shaders.computeShader }
        };
        CreateBindGroupLayout(inDevice, shaderInstancePacks);
        CreateRHIPipelineLayout(inDevice);
    }

    PipelineLayout::PipelineLayout(RHI::Device& inDevice, const RasterPipelineLayoutDesc& inDesc, size_t inHash)
        : hash(inHash)
    {
        const std::vector<ShaderInstancePack> shaderInstancePacks = {
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
        std::unordered_map<uint8_t, RBindingMap> layoutBindingMaps;
        for (const auto& [stage, instance] : shaderInstancePack) {
            if (!instance->Valid()) {
                continue;
            }
            Assert(instance->reflectionData != nullptr);

            for (const auto& resourceBindings = instance->reflectionData->resourceBindings;
                const auto& [bindingName, layoutIndexAndBinding] : resourceBindings) {
                const auto& [layoutIndex, binding] = layoutIndexAndBinding;

                if (!layoutBindingMaps.contains(layoutIndex)) {
                    layoutBindingMaps[layoutIndex] = {};
                }
                auto& layoutBindingMap = layoutBindingMaps[layoutIndex];
                layoutBindingMap.emplace(std::make_pair(bindingName, std::make_pair(stage, binding)));
            }
        }

        for (const auto& [layoutIndex, binding] : layoutBindingMaps) {
            BindGroupLayoutDesc desc;
            desc.layoutIndex = layoutIndex;
            desc.binding = binding;
            bindGroupLayouts[layoutIndex] = Common::UniquePtr(new BindGroupLayout(device, desc));
        }
    }

    void PipelineLayout::CreateRHIPipelineLayout(RHI::Device& device)
    {
        std::vector<const RHI::BindGroupLayout*> tBindGroupLayouts;
        tBindGroupLayouts.reserve(bindGroupLayouts.size());
        for (const auto& [layoutIndex, layout] : bindGroupLayouts) {
            tBindGroupLayouts.emplace_back(layout->GetRHI());
        }

        RHI::PipelineLayoutCreateInfo createInfo;
        createInfo.bindGroupLayouts = tBindGroupLayouts;
        rhiHandle = device.CreatePipelineLayout(createInfo);
    }

    PipelineLayout::~PipelineLayout() = default;

    BindGroupLayout* PipelineLayout::GetBindGroupLayout(const uint8_t layoutIndex) const
    {
        const auto iter = bindGroupLayouts.find(layoutIndex);
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

    ComputePipelineState::ComputePipelineState(RHI::Device& inDevice, const ComputePipelineStateDesc& inDesc, const size_t inHash)
        : hash(inHash)
    {
        const ComputePipelineLayoutDesc desc = { inDesc.shaders };
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

    std::mutex SamplerCache::mutex = std::mutex();

    SamplerCache& SamplerCache::Get(RHI::Device& device)
    {
        static std::unordered_map<RHI::Device*, Common::UniquePtr<SamplerCache>> map;

        std::unique_lock lock(mutex);
        const auto iter = map.find(&device);
        if (iter == map.end()) {
            map[&device] = Common::UniquePtr(new SamplerCache(device));
        }
        return *map[&device];
    }

    SamplerCache::SamplerCache(RHI::Device& inDevice)
        : device(inDevice)
    {
    }

    SamplerCache::~SamplerCache() = default;

    Sampler* SamplerCache::GetOrCreate(const RSamplerDesc& desc)
    {
        const size_t hash = Common::HashUtils::CityHash(&desc, sizeof(RSamplerDesc));
        if (const auto iter = samplers.find(hash);
            iter == samplers.end()) {
            samplers[hash] = Common::UniquePtr(new Sampler(device, desc));
        }
        return samplers[hash].Get();
    }

    std::mutex PipelineCache::mutex = std::mutex();

    PipelineCache& PipelineCache::Get(RHI::Device& device)
    {
        static std::unordered_map<RHI::Device*, Common::UniquePtr<PipelineCache>> map;

        std::unique_lock lock(mutex);
        if (const auto iter = map.find(&device);
            iter == map.end()) {
            map[&device] = Common::UniquePtr(new PipelineCache(device));
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
        PipelineLayoutCache::Get(device).Invalidate();
    }

    ComputePipelineState* PipelineCache::GetOrCreate(const ComputePipelineStateDesc& desc)
    {
        const auto hash = desc.Hash();
        if (const auto iter = computePipelines.find(hash);
            iter == computePipelines.end()) {
            computePipelines[hash] = Common::UniquePtr(new ComputePipelineState(device, desc, hash));
        }
        return computePipelines[hash].Get();
    }

    RasterPipelineState* PipelineCache::GetOrCreate(const RasterPipelineStateDesc& desc)
    {
        const auto hash = desc.Hash();
        if (const auto iter = rasterPipelines.find(hash);
            iter == rasterPipelines.end()) {
            rasterPipelines[hash] = Common::UniquePtr(new RasterPipelineState(device, desc, hash));
        }
        return rasterPipelines[hash].Get();
    }

    std::mutex ResourceViewCache::mutex = std::mutex();

    ResourceViewCache& ResourceViewCache::Get(RHI::Device& device)
    {
        static std::unordered_map<RHI::Device*, Common::UniquePtr<ResourceViewCache>> map;

        std::unique_lock lock(mutex);
        if (!map.contains(&device)) {
            map.emplace(std::make_pair(&device, Common::UniquePtr(new ResourceViewCache(device))));
        }
        return *map.at(&device);
    }

    ResourceViewCache::ResourceViewCache(RHI::Device& inDevice)
        : device(inDevice)
    {
    }

    ResourceViewCache::~ResourceViewCache() = default;

    RHI::BufferView* ResourceViewCache::GetOrCreate(RHI::Buffer* buffer, const RHI::BufferViewCreateInfo& inDesc)
    {
        auto& views = bufferViewCaches[buffer].views;

        auto hash = inDesc.Hash();
        if (const auto iter = views.find(hash);
            iter == views.end()) {
            views.emplace(std::make_pair(hash, Common::UniquePtr(buffer->CreateBufferView(inDesc))));
        }
        return views.at(hash).Get();
    }

    RHI::TextureView* ResourceViewCache::GetOrCreate(RHI::Texture* texture, const RHI::TextureViewCreateInfo& inDesc)
    {
        auto& views = textureViewCaches[texture].views;

        auto hash = inDesc.Hash();
        if (const auto iter = views.find(hash);
            iter == views.end()) {
            views.emplace(std::make_pair(hash, Common::UniquePtr(texture->CreateTextureView(inDesc))));
        }
        return views.at(hash).Get();
    }

    void ResourceViewCache::Invalidate(RHI::Buffer* buffer) // NOLINT
    {
        if (const auto iter = bufferViewCaches.find(buffer);
            iter != bufferViewCaches.end()) {
            iter->second.valid = false;
        }
    }

    void ResourceViewCache::Invalidate(RHI::Texture* texture) // NOLINT
    {
        if (const auto iter = textureViewCaches.find(texture);
            iter != textureViewCaches.end()) {
            iter->second.valid = false;
        }
    }

    void ResourceViewCache::Forfeit()
    {
        const auto forfeitCaches = [](auto& caches) -> void { // NOLINT
            const auto currentFrameNumber = Core::ThreadContext::FrameNumber();

            std::vector<typename std::decay_t<decltype(caches)>::key_type> resourcesToRelease;
            resourcesToRelease.reserve(caches.size());

            for (auto& [resource, cache] : caches) {
                auto& [valid, lastUsedFrame, views] = cache;

                if (valid) {
                    lastUsedFrame = currentFrameNumber;
                } else if (currentFrameNumber - lastUsedFrame > Internal::resourceViewCacheReleaseFrameLatency) {
                    resourcesToRelease.emplace_back(resource);
                }
            }

            for (auto* resourceToRelease : resourcesToRelease) {
                caches.erase(resourceToRelease);
            }
        };

        forfeitCaches(bufferViewCaches);
        forfeitCaches(textureViewCaches);
    }

    std::mutex BindGroupCache::mutex = std::mutex();

    BindGroupCache& BindGroupCache::Get(RHI::Device& device)
    {
        static std::unordered_map<RHI::Device*, Common::UniquePtr<BindGroupCache>> map;

        std::unique_lock lock(mutex);
        if (!map.contains(&device)) {
            map.emplace(std::make_pair(&device, Common::UniquePtr(new BindGroupCache(device))));
        }
        return *map.at(&device);
    }

    BindGroupCache::~BindGroupCache() = default;

    RHI::BindGroup* BindGroupCache::Allocate(const RHI::BindGroupCreateInfo& inCreateInfo)
    {
        const auto& [ptr, frameNumber] = bindGroups.emplace_back(device.CreateBindGroup(inCreateInfo), Core::ThreadContext::FrameNumber());
        return ptr.Get();
    }

    void BindGroupCache::Invalidate()
    {
        bindGroups.clear();
    }

    void BindGroupCache::Forfeit()
    {
        const auto currentFrame = Core::ThreadContext::FrameNumber();

        for (auto i = 0; i < bindGroups.size();) {
            const auto& [ptr, lastUsedFrame] = bindGroups[i];
            if (currentFrame - lastUsedFrame > Internal::bindGroupCacheReleaseFrameLatency) { // NOLINT
                bindGroups.erase(bindGroups.begin() + i);
            } else {
                i++;
            }
        }
    }

    BindGroupCache::BindGroupCache(RHI::Device& inDevice)
        : device(inDevice)
    {
    }
} // namespace Render
