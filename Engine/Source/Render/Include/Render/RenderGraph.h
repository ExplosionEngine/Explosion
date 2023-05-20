//
// Created by johnk on 2023/2/10.
//

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <optional>

#include <Common/Hash.h>
#include <Common/Debug.h>
#include <RHI/RHI.h>
#include <Render/Pipeline.h>

namespace Render {
    class RGResource;
    class RGBuffer;
    class RGTexture;
    class RGBufferView;
    class RGTextureView;
    class RGSampler;
    class RGPass;
    class RGPassBuilder;
    class RGCopyPassBuilder;
    class RGComputePassBuilder;
    class RGRasterPassBuilder;

    enum class RGResourceType {
        buffer,
        texture,
        bufferView,
        textureView,
        sampler,
        bindGroup,
        max
    };

    using RGTransitionResType = RHI::ResourceType;
    using RGBufferState = RHI::BufferState;
    using RGTextureState = RHI::TextureState;

    enum class RGPassType {
        copy,
        compute,
        raster,
        max
    };

    enum class RGResourceAccessType {
        read,
        write,
        max
    };

    struct RGBufferTransition {
        RGBuffer* buffer;
        RGBufferState before;
        RGBufferState after;
    };

    struct RGTextureTransition {
        RGTexture* texture;
        RGTextureState before;
        RGTextureState after;
    };

    struct RGResTransition {
        RGTransitionResType resType;
        union {
            RGBufferTransition buffer;
            RGTextureTransition texture;
        };

        static RGResTransition Buffer(RGBuffer* inBuffer, RGBufferState inBeforeState, RGBufferState inAfterState);
        static RGResTransition Texture(RGTexture* inTexture, RGTextureState inBeforeState, RGTextureState inAfterState);
    };

    struct RGBufferDesc {
        size_t size;
        RHI::BufferUsageFlags usages;

        static RGBufferDesc Create(size_t size, RHI::BufferUsageFlags usages);
    };

    struct RGTextureDesc {
        RHI::Extent<3> extent;
        uint8_t mipLevels;
        uint8_t samples;
        RHI::TextureDimension dimension;
        RHI::PixelFormat format;
        RHI::TextureUsageFlags usages;

        static RGTextureDesc Create1D(uint32_t length, RHI::PixelFormat format, RHI::TextureUsageFlags usages, uint8_t mipLevels = 1, uint8_t samples = 1);
        static RGTextureDesc Create2D(uint32_t width, uint32_t height, RHI::PixelFormat format, RHI::TextureUsageFlags usages, uint32_t layers = 1, uint8_t mipLevels = 1, uint8_t samples = 1);
        static RGTextureDesc Create3D(uint32_t width, uint32_t height, uint32_t depth, RHI::PixelFormat format, RHI::TextureUsageFlags usages, uint8_t mipLevels = 1, uint8_t samples = 1);
    };

    struct RGVertexBufferDesc {
        size_t stride;
    };

    struct RGIndexBufferDesc {
        RHI::IndexFormat format;
    };

    struct RGBufferViewDesc {
        size_t offset;
        size_t size;
        union {
            RGVertexBufferDesc vertex;
            RGIndexBufferDesc index;
        };

        static RGBufferViewDesc Create(size_t size, size_t offset = 0);
        static RGBufferViewDesc CreateVertex(size_t stride, size_t size, size_t offset = 0);
        static RGBufferViewDesc CreateIndex(RHI::IndexFormat indexFormat, size_t size, size_t offset = 0);
        static std::pair<RGBuffer*, RGBufferViewDesc> Create(RGBuffer* buffer, size_t size = UINT64_MAX, size_t offset = 0);
        static std::pair<RGBuffer*, RGBufferViewDesc> CreateVertex(RGBuffer* buffer, size_t stride, size_t size = UINT64_MAX, size_t offset = 0);
        static std::pair<RGBuffer*, RGBufferViewDesc> CreateIndex(RGBuffer* buffer, RHI::IndexFormat indexFormat, size_t size = UINT64_MAX, size_t offset = 0);
    };

    struct RGTextureViewDesc {
        RHI::TextureViewDimension dimension;
        RHI::TextureAspect aspect;
        uint8_t baseMipLevel;
        uint8_t mipLevelNum;
        uint8_t baseArrayLayer;
        uint8_t arrayLayerNum;

        static RGTextureViewDesc Create1D(RHI::TextureAspect aspect = RHI::TextureAspect::color, uint8_t baseMipLevel = 0, uint8_t mipLevelNum = 1);
        static RGTextureViewDesc Create2D(RHI::TextureAspect aspect = RHI::TextureAspect::color, uint8_t baseMipLevel = 0, uint8_t mipLevelNum = 1);
        static RGTextureViewDesc Create2DArray(RHI::TextureAspect aspect = RHI::TextureAspect::color, uint8_t baseArrayLayer = 0, uint8_t arrayLayerNum = 1, uint8_t baseMipLevel = 0, uint8_t mipLevelNum = 1);
        static RGTextureViewDesc CreateCube(RHI::TextureAspect aspect = RHI::TextureAspect::color, uint8_t baseMipLevel = 0, uint8_t mipLevelNum = 1);
        static RGTextureViewDesc CreateCubeArray(RHI::TextureAspect aspect = RHI::TextureAspect::color, uint8_t baseCubemapIndex = 0, uint8_t cubemapNum = 1, uint8_t baseMipLevel = 0, uint8_t mipLevelNum = 1);
        static RGTextureViewDesc Create3D(RHI::TextureAspect aspect = RHI::TextureAspect::color, uint8_t baseMipLevel = 0, uint8_t mipLevelNum = 1);
        static std::pair<RGTexture*, RGTextureViewDesc> Create1D(RGTexture* texture, RHI::TextureAspect aspect = RHI::TextureAspect::color, uint8_t baseMipLevel = 0, uint8_t mipLevelNum = UINT8_MAX);
        static std::pair<RGTexture*, RGTextureViewDesc> Create2D(RGTexture* texture, RHI::TextureAspect aspect = RHI::TextureAspect::color, uint8_t baseMipLevel = 0, uint8_t mipLevelNum = UINT8_MAX);
        static std::pair<RGTexture*, RGTextureViewDesc> Create2DArray(RGTexture* texture, RHI::TextureAspect aspect = RHI::TextureAspect::color, uint8_t baseArrayLayer = 0, uint8_t arrayLayerNum = UINT8_MAX, uint8_t baseMipLevel = 0, uint8_t mipLevelNum = UINT8_MAX);
        static std::pair<RGTexture*, RGTextureViewDesc> CreateCube(RGTexture* texture, RHI::TextureAspect aspect = RHI::TextureAspect::color, uint8_t baseMipLevel = 0, uint8_t mipLevelNum = UINT8_MAX);
        static std::pair<RGTexture*, RGTextureViewDesc> CreateCubeArray(RGTexture* texture, RHI::TextureAspect aspect = RHI::TextureAspect::color, uint8_t baseCubemapIndex = 0, uint8_t cubemapNum = UINT8_MAX, uint8_t baseMipLevel = 0, uint8_t mipLevelNum = UINT8_MAX);
        static std::pair<RGTexture*, RGTextureViewDesc> Create3D(RGTexture* texture, RHI::TextureAspect aspect = RHI::TextureAspect::color, uint8_t baseMipLevel = UINT8_MAX, uint8_t mipLevelNum = UINT8_MAX);
    };

    struct RGBindItem {
        RHI::BindingType type;
        union {
            RGSampler* sampler;
            RGTextureView* textureView;
            RGBufferView* bufferView;
        };

        static RGBindItem Sampler(RGSampler* sampler);
        static RGBindItem Texture(RGTextureView* textureView);
        static RGBindItem StorageTexture(RGTextureView* textureView);
        static RGBindItem UniformBuffer(RGBufferView* bufferView);
        static RGBindItem StorageBuffer(RGBufferView* bufferView);
        static std::pair<std::string, RGBindItem> Sampler(std::string name, RGSampler* sampler);
        static std::pair<std::string, RGBindItem> Texture(std::string name, RGTextureView* textureView);
        static std::pair<std::string, RGBindItem> StorageTexture(std::string name, RGTextureView* textureView);
        static std::pair<std::string, RGBindItem> UniformBuffer(std::string name, RGBufferView* bufferView);
        static std::pair<std::string, RGBindItem> StorageBuffer(std::string name, RGBufferView* bufferView);
    };

    struct RGBindGroupDesc {
        RHI::BindGroupLayout* layout;
        std::vector<std::pair<std::string, RGBindItem>> items;

        template <typename... B>
        static RGBindGroupDesc Create(BindGroupLayout* layout, B&&... items)
        {
            RGBindGroupDesc result;
            result.layout = layout->GetRHI();
            result.items.reserve(sizeof...(items));
            (void) std::initializer_list<int> { ([&]() -> void { result.items.emplace_back(std::forward<B>(items)); }(), 0)... };
            return result;
        }
    };

    struct RGColorAttachment {
        RGTextureView* view;
        RGTextureView* resolve;
        RHI::ColorNormalized<4> clearValue;
        RHI::LoadOp loadOp;
        RHI::StoreOp storeOp;
    };

    struct RGDepthStencilAttachment {
        RGTextureView* view;
        float depthClearValue;
        RHI::LoadOp depthLoadOp;
        RHI::StoreOp depthStoreOp;
        bool depthReadOnly;
        uint32_t stencilClearValue;
        RHI::LoadOp stencilLoadOp;
        RHI::StoreOp stencilStoreOp;
        bool stencilReadOnly;
    };

    struct RGRasterPassDesc {
        std::vector<RGColorAttachment> colorAttachments;
        std::optional<RGDepthStencilAttachment> depthStencilAttachment;
    };

    class RGResource {
    public:
        virtual ~RGResource();

        bool IsExternal() const;
        bool IsCulled() const;
        const std::string& GetName() const;
        bool CanAccessRHI() const;
        RGResource* GetParent() const;
        virtual RGResourceType GetType() = 0;

    protected:
        RGResource(std::string inName, bool inIsExternal, RGResource* inParent = nullptr);

        virtual void Devirtualize(RHI::Device& device) = 0;
        virtual void Destroy() = 0;

        void SetRHIAccess(bool inRhiAccess);

    private:
        friend class RenderGraph;

        void SetCulled(bool inCulled);

        std::string name;
        RGResource* parent;
        bool isExternal;
        bool rhiAccess;
        bool isCulled;
    };

    class RGBuffer : public RGResource {
    public:
        RGBuffer(std::string inName, const RGBufferDesc& inDesc);
        RGBuffer(std::string inName, RHI::Buffer* inBuffer);
        ~RGBuffer() override;

        RGResourceType GetType() override;
        void Devirtualize(RHI::Device& device) override;
        void Destroy() override;
        RHI::Buffer* GetRHI() const;
        // TODO support external desc register
        const RGBufferDesc& GetDesc() const;

        template <typename D>
        void UploadData(const D& data)
        {
            Assert((desc.usages & RHI::BufferUsageBits::uniform) != 0 && CanAccessRHI() && sizeof(D) == desc.size);
            void* mapResult = rhiHandle->Map(RHI::MapMode::write, 0, desc.size);
            memcpy(mapResult, &data, desc.size);
            rhiHandle->UnMap();
        }

    private:
        RGBufferDesc desc;
        RHI::Buffer* rhiHandle;
    };

    class RGTexture : public RGResource {
    public:
        RGTexture(std::string inName, const RGTextureDesc& inDesc);
        RGTexture(std::string inName, RHI::Texture* inTexture);
        ~RGTexture() override;

        RGResourceType GetType() override;
        void Devirtualize(RHI::Device& device) override;
        void Destroy() override;
        RHI::Texture* GetRHI() const;
        // TODO support external desc register
        const RGTextureDesc& GetDesc() const;

    private:
        RGTextureDesc desc;
        RHI::Texture* rhiHandle;
    };

    class RGBufferView : public RGResource {
    public:
        explicit RGBufferView(const std::pair<RGBuffer*, RGBufferViewDesc>& bufferAndViewDesc);
        RGBufferView(RGBuffer* inBuffer, const RGBufferViewDesc& inDesc);
        RGBufferView(std::string inName, const std::pair<RGBuffer*, RGBufferViewDesc>& bufferAndViewDesc);
        RGBufferView(std::string inName, RGBuffer* inBuffer, const RGBufferViewDesc& inDesc);
        RGBufferView(std::string inName, RHI::BufferView* inBufferView);
        ~RGBufferView() override;

        RGResourceType GetType() override;
        void Devirtualize(RHI::Device& device) override;
        void Destroy() override;
        RHI::BufferView* GetRHI() const;
        RGBuffer* GetBuffer() const;
        // TODO support external desc register
        const RGBufferViewDesc& GetDesc() const;

    private:
        RGBufferViewDesc desc;
        RGBuffer* buffer;
        RHI::BufferView* rhiHandle;
    };

    class RGTextureView : public RGResource {
    public:
        explicit RGTextureView(const std::pair<RGTexture*, RGTextureViewDesc>& textureAndViewDesc);
        RGTextureView(RGTexture* inTexture, const RGTextureViewDesc& inDesc);
        RGTextureView(std::string inName,const std::pair<RGTexture*, RGTextureViewDesc>& textureAndViewDesc);
        RGTextureView(std::string inName, RGTexture* inTexture, const RGTextureViewDesc& inDesc);
        RGTextureView(std::string inName, RHI::TextureView* inTextureView);
        ~RGTextureView() override;

        RGResourceType GetType() override;
        void Devirtualize(RHI::Device& device) override;
        void Destroy() override;
        RHI::TextureView* GetRHI() const;
        RGTexture* GetTexture() const;
        // TODO support external desc register
        const RGTextureViewDesc& GetDesc() const;

    private:
        RGTextureViewDesc desc;
        RGTexture* texture;
        RHI::TextureView* rhiHandle;
    };

    class RGBindGroup : public RGResource {
    public:
        explicit RGBindGroup(RGBindGroupDesc inDesc);
        explicit RGBindGroup(RHI::BindGroup* inBindGroup);
        RGBindGroup(std::string inName, RGBindGroupDesc inDesc);
        RGBindGroup(std::string inName, RHI::BindGroup* inBindGroup);
        ~RGBindGroup() override;

        RGResourceType GetType() override;
        void Devirtualize(RHI::Device &device) override;
        void Destroy() override;
        RHI::BindGroup* GetRHI() const;
        // TODO support external desc register
        const RGBindGroupDesc& GetDesc() const;

    private:
        RGBindGroupDesc desc;
        RHI::BindGroup* rhiHandle;
    };

    class RGPass {
    public:
        virtual ~RGPass();

    protected:
        explicit RGPass(std::string inName);

        virtual RGPassType GetType() = 0;

    private:
        friend class RenderGraph;
        friend class RGPassBuilder;

        std::string name;
        std::unordered_set<RGResource*> reads;
        std::unordered_set<RGResource*> writes;
    };

    class RGCopyPass : public RGPass {
    public:
        ~RGCopyPass() override;

    protected:
        explicit RGCopyPass(std::string inName);

        RGPassType GetType() override;
        virtual void Setup(RGCopyPassBuilder& builder) = 0;
        virtual void Execute(RHI::CommandEncoder& encoder) = 0;

    private:
        friend class RenderGraph;
        friend class RGCopyPassBuilder;

        bool isAsyncCopy;
    };

    class RGComputePass : public RGPass {
    public:
        ~RGComputePass() override;

    protected:
        explicit RGComputePass(std::string inName);

        RGPassType GetType() override;
        virtual void Setup(RGComputePassBuilder& builder) = 0;
        virtual void Execute(RHI::ComputePassCommandEncoder& encoder) = 0;

    private:
        friend class RenderGraph;
        friend class RGComputePassBuilder;

        bool isAsyncCompute;
    };

    class RGRasterPass : public RGPass {
    public:
        ~RGRasterPass() override;

    protected:
        explicit RGRasterPass(std::string inName);

        RGPassType GetType() override;
        virtual void Setup(RGRasterPassBuilder& builder) = 0;
        virtual void Execute(RHI::GraphicsPassCommandEncoder& encoder) = 0;

    private:
        friend class RenderGraph;
        friend class RGRasterPassBuilder;

        RGRasterPassDesc passDesc;
    };

    using RGCopyPassExecuteFunc = std::function<void(RHI::CommandEncoder&)>;
    using RGCopyPassSetupFunc = std::function<RGCopyPassExecuteFunc(RGCopyPassBuilder&)>;
    using RGComputePassExecuteFunc = std::function<void(RHI::ComputePassCommandEncoder&)>;
    using RGComputePassSetupFunc = std::function<RGComputePassExecuteFunc(RGComputePassBuilder&)>;
    using RGRasterPassExecuteFunc = std::function<void(RHI::GraphicsPassCommandEncoder&)>;
    using RGRasterPassSetupFunc = std::function<RGRasterPassExecuteFunc(RGRasterPassBuilder&)>;

    class RGFuncCopyPass : public RGCopyPass {
    public:
        RGFuncCopyPass(std::string inName, RGCopyPassSetupFunc inSetupFunc);
        ~RGFuncCopyPass() override;

        void Setup(RGCopyPassBuilder& builder) override;
        void Execute(RHI::CommandEncoder& encoder) override;

    private:
        friend class RGCopyPassBuilder;

        RGCopyPassSetupFunc setupFunc;
        RGCopyPassExecuteFunc executeFunc;
    };

    class RGFuncComputePass : public RGComputePass {
    public:
        RGFuncComputePass(std::string inName, RGComputePassSetupFunc inSetupFunc);
        ~RGFuncComputePass() override;

        void Setup(RGComputePassBuilder& builder) override;
        void Execute(RHI::ComputePassCommandEncoder &encoder) override;

    private:
        RGComputePassSetupFunc setupFunc;
        RGComputePassExecuteFunc executeFunc;
    };

    class RGFuncRasterPass : public RGRasterPass {
    public:
        RGFuncRasterPass(std::string inName, RGRasterPassSetupFunc inSetupFunc);
        ~RGFuncRasterPass() override;

        void Setup(RGRasterPassBuilder& builder) override;
        void Execute(RHI::GraphicsPassCommandEncoder &encoder) override;

    private:
        RGRasterPassSetupFunc setupFunc;
        RGRasterPassExecuteFunc executeFunc;
    };

    class RenderGraph {
    public:
        explicit RenderGraph(RHI::Device& inDevice);
        ~RenderGraph();

        void AddCopyPass(RGCopyPass* inPass);
        void AddComputePass(RGComputePass* inPass);
        void AddRasterPass(RGRasterPass* inPass);
        void AddCopyPass(std::string inName, RGCopyPassSetupFunc inSetupFunc);
        void AddComputePass(std::string inName, RGComputePassSetupFunc inSetupFunc);
        void AddRasterPass(std::string inName, RGRasterPassSetupFunc inSetupFunc);

        RHI::Device& GetDevice();
        void Setup();
        void Compile();
        void Execute(RHI::Fence* mainFence, RHI::Fence* asyncComputeFence = nullptr, RHI::Fence* asyncCopyFence = nullptr);

    private:
        using LastResStates = std::unordered_map<RGResource*, std::pair<RGPassType, RGResourceAccessType>>;

        struct ResPassPtrPairHash {
            size_t operator()(const std::pair<RGResource*, RGPass*>& pair) const
            {
                return Common::HashUtils::CityHash(&pair, sizeof(pair));
            }
        };

        friend class RGPassBuilder;

        static void ExecuteCopyPass(RHI::CommandEncoder* encoder, RGCopyPass* copyPass);
        static void ExecuteComputePass(RHI::CommandEncoder* encoder, RGComputePass* computePass);
        static void ExecuteRasterPass(RHI::CommandEncoder* encoder, RGRasterPass* rasterPass);
        static RGBuffer* GetActualBufferRes(RGResource* res);
        static RGTexture* GetActualTextureRes(RGResource* res);
        static RHI::BufferState ComputeBufferState(RGPassType passType, RGResourceAccessType accessType);
        static RHI::TextureState ComputeTextureState(RGPassType passType, RGResourceAccessType accessType);

        void ComputeResBarriers();
        void TransitionResources(RHI::CommandEncoder* encoder, RGPass* pass);

        template <RGResourceAccessType AT>
        void ComputeResTransitionsByAccessGroup(RGPass* pass, const std::unordered_set<RGResource*>& resAccessGroup, LastResStates& lastResStates)
        {
            for (auto* res : resAccessGroup) {
                if (res->isCulled) {
                    continue;
                }

                const auto resType = res->GetType();
                const bool isActualResBuffer = resType == RGResourceType::buffer || resType == RGResourceType::bufferView;
                const bool isActualResTexture = resType == RGResourceType::texture || resType == RGResourceType::textureView;

                auto iter = lastResStates.find(res);
                if (isActualResBuffer) {
                    auto before = iter == lastResStates.end()
                        ? ComputeBufferState(RGPassType::max, RGResourceAccessType::max)
                        : ComputeBufferState(iter->second.first, iter->second.second);
                    auto after = ComputeBufferState(pass->GetType(), AT);
                    resTransitionMap[std::make_pair(res, pass)] = RGResTransition::Buffer(GetActualBufferRes(res), before, after);
                } else if (isActualResTexture) {
                    auto before = iter == lastResStates.end()
                        ? ComputeTextureState(RGPassType::max, RGResourceAccessType::max)
                        : ComputeTextureState(iter->second.first, iter->second.second);
                    auto after = ComputeTextureState(pass->GetType(), AT);
                    resTransitionMap[std::make_pair(res, pass)] = RGResTransition::Texture(GetActualTextureRes(res), before, after);
                }
            }
        }

        template <RGResourceAccessType AT>
        void UpdateLastResStatesByAccessGroup(RGPassType passType, const std::unordered_set<RGResource*>& resAccessGroup, LastResStates& lastResStates)
        {
            for (auto* res : resAccessGroup) {
                lastResStates[res] = std::make_pair(passType, AT);
            }
        }

        RHI::Device& device;
        std::vector<Common::UniqueRef<RGResource>> resources;
        std::vector<Common::UniqueRef<RGPass>> passes;
        std::unordered_map<std::pair<RGResource*, RGPass*>, RGResTransition, ResPassPtrPairHash> resTransitionMap;
    };

    class RGPassBuilder {
    public:
        RGPassBuilder(RenderGraph& inGraph, RGPass& inPass);
        virtual ~RGPassBuilder();

        template <typename... Args>
        RGBuffer* CreateBuffer(Args&&... args)
        {
            return Create<RGBuffer>(std::forward<Args>(args)...);
        }

        template <typename... Args>
        RGTexture* CreateTexture(Args&&... args)
        {
            return Create<RGTexture>(std::forward<Args>(args)...);
        }

        template <typename... Args>
        RGBufferView* CreateBufferView(Args&&... args)
        {
            return Create<RGBufferView>(std::forward<Args>(args)...);
        }

        template <typename... Args>
        RGTextureView* CreateTextureView(Args&&... args)
        {
            return Create<RGTextureView>(std::forward<Args>(args)...);
        }

        template <typename... Args>
        RGSampler* CreateSampler(Args&&... args)
        {
            return Create<RGSampler>(std::forward<Args>(args)...);
        }

        template <typename... Args>
        RGBindGroup* AllocateBindGroup(Args&&... args)
        {
            RGBindGroup* bindGroup = Create<RGBindGroup>(std::forward<Args>(args)...);
            MarkDependenciesFromBindGroup(bindGroup);
            return bindGroup;
        }

        void MarkAsConsumed(RGResource* res);
        RHI::Device& GetDevice();

    protected:
        template <typename R, typename... Args>
        R* Create(Args&&... args)
        {
            graph.resources.emplace_back(new R(std::forward<Args>(args)...));
            return static_cast<R*>(graph.resources.back().Get());
        }

        RenderGraph& graph;
        RGPass& pass;

    private:
        void MarkDependenciesFromBindGroup(RGBindGroup* bindGroup);
    };

    class RGCopyPassBuilder : public RGPassBuilder {
    public:
        ~RGCopyPassBuilder() override;

        void SetAsyncCopy(bool inAsyncCopy);

    private:
        friend class RenderGraph;

        RGCopyPassBuilder(RenderGraph& inGraph, RGCopyPass& inPass);

        RGCopyPass& copyPass;
    };

    class RGComputePassBuilder : public RGPassBuilder {
    public:
        ~RGComputePassBuilder() override;

        void SetAsyncCompute(bool inAsyncCompute);

    private:
        friend class RenderGraph;

        RGComputePassBuilder(RenderGraph& inGraph, RGComputePass& inPass);

        RGComputePass& computePass;
    };

    class RGRasterPassBuilder : public RGPassBuilder {
    public:
        ~RGRasterPassBuilder() override;

        void SetPassDesc(const RGRasterPassDesc& inDesc);

    private:
        friend class RenderGraph;

        RGRasterPassBuilder(RenderGraph& inGraph, RGRasterPass& inPass);

        RGRasterPass& rasterPass;
    };
}
