//
// Created by johnk on 2023/2/10.
//

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>

#include <Common/Hash.h>
#include <RHI/RHI.h>

namespace Render {
    class RGResource;
    class RGBuffer;
    class RGTexture;
    class RGBufferView;
    class RGTextureView;
    class RGPass;
    class RGPassBuilder;
    class RGCopyPassBuilder;
    class RGComputePassBuilder;
    class RGRasterPassBuilder;

    enum class RGResourceType {
        BUFFER,
        TEXTURE,
        BUFFER_VIEW,
        TEXTURE_VIEW,
        SAMPLER,
        MAX
    };

    using RGTransitionResType = RHI::ResourceType;
    using RGBufferState = RHI::BufferState;
    using RGTextureState = RHI::TextureState;

    enum class RGPassType {
        COPY,
        COMPUTE,
        RASTER,
        MAX
    };

    enum class RGResourceAccessType {
        READ,
        WRITE,
        MAX
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

        static RGResTransition Buffer(RGBuffer* inBuffer, RGBufferState inBeforeState, RGBufferState inAfterState)
        {
            RGResTransition result;
            result.resType = RGTransitionResType::BUFFER;
            result.buffer.buffer = inBuffer;
            result.buffer.before = inBeforeState;
            result.buffer.after = inAfterState;
            return result;
        }

        static RGResTransition Texture(RGTexture* inTexture, RGTextureState inBeforeState, RGTextureState inAfterState)
        {
            RGResTransition result;
            result.resType = RGTransitionResType::TEXTURE;
            result.texture.texture = inTexture;
            result.texture.before = inBeforeState;
            result.texture.after = inAfterState;
            return result;
        }
    };

    class RGResource {
    public:
        virtual ~RGResource();

    protected:
        RGResource(std::string inName, bool inIsExternal);

        virtual RGResourceType GetType() = 0;
        virtual void Devirtualize(RHI::Device& device) = 0;
        virtual void Destroy() = 0;

        std::string name;
        bool isExternal;
        bool rhiAccess;
        bool isCulled;

    private:
        friend class RenderGraph;

        void SetCulled(bool inCulled);
        void SetRHIAccess(bool inRhiAccess);
    };

    struct RGBufferDesc {
        size_t size;
        RHI::BufferUsageFlags usages;
    };

    struct RGTextureDesc {
        RHI::Extent<3> extent;
        uint8_t mipLevels;
        uint8_t samples;
        RHI::TextureDimension dimension;
        RHI::PixelFormat format;
        RHI::TextureUsageFlags usages;
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
    };

    struct RGTextureViewDesc {
        RHI::TextureViewDimension dimension;
        RHI::TextureAspect aspect;
        uint8_t baseMipLevel;
        uint8_t mipLevelNum;
        uint8_t baseArrayLayer;
        uint8_t arrayLayerNum;
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

    struct RGComputePassDesc {
        RHI::ComputePipeline* pipeline;
    };

    struct RGRasterPassDesc {
        RHI::GraphicsPipeline* pipeline;
        std::vector<RGColorAttachment> colorAttachments;
        std::optional<RGDepthStencilAttachment> depthStencilAttachment;
    };

    class RGBuffer : public RGResource {
    public:
        RGBuffer(std::string inName, RGBufferDesc inDesc);
        RGBuffer(std::string inName, RHI::Buffer* inBuffer);
        ~RGBuffer() override;

        RGResourceType GetType() override;
        void Devirtualize(RHI::Device& device) override;
        void Destroy() override;
        RHI::Buffer* GetRHI();

    private:
        RGBufferDesc desc;
        RHI::Buffer* rhiHandle;
    };

    class RGTexture : public RGResource {
    public:
        RGTexture(std::string inName, RGTextureDesc inDesc);
        RGTexture(std::string inName, RHI::Texture* inTexture);
        ~RGTexture() override;

        RGResourceType GetType() override;
        void Devirtualize(RHI::Device& device) override;
        void Destroy() override;
        RHI::Texture* GetRHI();

    private:
        RGTextureDesc desc;
        RHI::Texture* rhiHandle;
    };

    class RGBufferView : public RGResource {
    public:
        RGBufferView(std::string inName, RGBuffer* inBuffer, RGBufferViewDesc inDesc);
        RGBufferView(std::string inName, RHI::BufferView* inBufferView);
        ~RGBufferView() override;

        RGResourceType GetType() override;
        void Devirtualize(RHI::Device& device) override;
        void Destroy() override;
        RHI::BufferView* GetRHI();
        RGBuffer* GetBuffer();

    private:
        RGBufferViewDesc desc;
        RGBuffer* buffer;
        RHI::BufferView* rhiHandle;
    };

    class RGTextureView : public RGResource {
    public:
        RGTextureView(std::string inName, RGTexture* inTexture, RGTextureViewDesc inDesc);
        RGTextureView(std::string inName, RHI::TextureView* inTextureView);
        ~RGTextureView() override;

        RGResourceType GetType() override;
        void Devirtualize(RHI::Device& device) override;
        void Destroy() override;
        RHI::TextureView* GetRHI();
        RGTexture* GetTexture();

    private:
        RGTextureViewDesc desc;
        RGTexture* texture;
        RHI::TextureView* rhiHandle;
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
        RGComputePassDesc passDesc;
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

        void Setup();
        void Compile();
        void Execute(RHI::Fence* mainFence, RHI::Fence* asyncFence);

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
                const bool isActualResBuffer = resType == RGResourceType::BUFFER || resType == RGResourceType::BUFFER_VIEW;
                const bool isActualResTexture = resType == RGResourceType::TEXTURE || resType == RGResourceType::TEXTURE_VIEW;

                auto iter = lastResStates.find(res);
                if (isActualResBuffer) {
                    auto before = iter == lastResStates.end()
                        ? ComputeBufferState(RGPassType::MAX, RGResourceAccessType::MAX)
                        : ComputeBufferState(iter->second.first, iter->second.second);
                    auto after = ComputeBufferState(pass->GetType(), AT);
                    resTransitionMap[std::make_pair(res, pass)] = RGResTransition::Buffer(GetActualBufferRes(res), before, after);
                } else if (isActualResTexture) {
                    auto before = iter == lastResStates.end()
                        ? ComputeTextureState(RGPassType::MAX, RGResourceAccessType::MAX)
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
        std::vector<std::unique_ptr<RGResource>> resources;
        std::vector<std::unique_ptr<RGPass>> passes;
        std::unordered_map<std::pair<RGResource*, RGPass*>, RGResTransition, ResPassPtrPairHash> resTransitionMap;
    };

    class RGPassBuilder {
    public:
        RGPassBuilder(RenderGraph& inGraph, RGPass& inPass);
        virtual ~RGPassBuilder();

        template <typename R, typename... Args>
        R* Create(Args&&... args)
        {
            graph.resources.emplace_back(new R(std::forward<Args>(args)...));
            return graph.resources.back().get();
        }

        template <typename R, typename... Args>
        R* RegisterExternal(Args&&... args)
        {
            graph.resources.emplace_back(new R(std::forward<Args>(args)...));
            return graph.resources.back().get();
        }

        template <typename R>
        void Read(R* resource)
        {
            CheckReadWriteOnce(resource);
            pass.reads.emplace(resource);
        }

        template <typename R>
        void Write(R* resource)
        {
            CheckReadWriteOnce(resource);
            pass.writes.emplace(resource);
        }

    protected:
        RenderGraph& graph;
        RGPass& pass;

    private:
        void CheckReadWriteOnce(RGResource* resource);
    };

    class RGCopyPassBuilder : public RGPassBuilder {
    public:
        ~RGCopyPassBuilder() override;

    private:
        friend class RenderGraph;

        RGCopyPassBuilder(RenderGraph& inGraph, RGCopyPass& inPass);

        RGCopyPass& copyPass;
    };

    class RGComputePassBuilder : public RGPassBuilder {
    public:
        ~RGComputePassBuilder() override;

        void SetAsyncCompute(bool inAsyncCompute);
        void SetPassDesc(const RGComputePassDesc& inDesc);

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
