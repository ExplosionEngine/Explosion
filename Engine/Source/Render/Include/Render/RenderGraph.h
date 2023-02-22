//
// Created by johnk on 2023/2/10.
//

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

#include <RHI/RHI.h>

namespace Render {
    class RGResource;
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

    enum class RGPassType {
        COPY,
        COMPUTE,
        RASTER,
        MAX
    };

    class RGResource {
    public:
        virtual ~RGResource();

    protected:
        RGResource(std::string inName, bool inIsExternal);

        virtual RGResourceType GetType() = 0;
        virtual void Devirtualize() = 0;
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
    };

    struct RGTextureDesc {
        RHI::Extent<3> extent;
        uint8_t mipLevels;
        uint8_t samples;
        RHI::TextureDimension dimension;
        RHI::PixelFormat format;
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
        uint8_t baseMipLevels;
        uint8_t mipLevelNum;
        uint8_t baseArrayLayer;
        uint8_t arrayLayerNum;
    };

    struct RGSamplerDesc {
        RHI::AddressMode addressModeU = RHI::AddressMode::CLAMP_TO_EDGE;
        RHI::AddressMode addressModeV = RHI::AddressMode::CLAMP_TO_EDGE;
        RHI::AddressMode addressModeW = RHI::AddressMode::CLAMP_TO_EDGE;
        RHI::FilterMode magFilter = RHI::FilterMode::NEAREST;
        RHI::FilterMode minFilter = RHI::FilterMode::NEAREST;
        RHI::FilterMode mipFilter = RHI::FilterMode::NEAREST;
        float lodMinClamp = 0;
        float lodMaxClamp = 32;
        RHI::ComparisonFunc comparisonFunc = RHI::ComparisonFunc::NEVER;
        uint8_t maxAnisotropy = 1;
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
        void Devirtualize() override;
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
        void Devirtualize() override;
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
        void Devirtualize() override;
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
        void Devirtualize() override;
        void Destroy() override;
        RHI::TextureView* GetRHI();
        RGTexture* GetTexture();

    private:
        RGTextureViewDesc desc;
        RGTexture* texture;
        RHI::TextureView* rhiHandle;
    };

    class RGSampler : public RGResource {
    public:
        RGSampler(std::string inName, RGSamplerDesc inDesc);
        RGSampler(std::string inName, RHI::Sampler* inSampler);
        ~RGSampler() override;

        RGResourceType GetType() override;
        void Devirtualize() override;
        void Destroy() override;
        RHI::Sampler* GetRHI();

    private:
        RGSamplerDesc desc;
        RHI::Sampler* rhiHandle;
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
        std::vector<RGResource*> reads;
        std::vector<RGResource*> writes;
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
        friend class RGPassBuilder;

        static RHI::BufferState SpeculateBufferStateFrom(RGPass* writePass);
        static RHI::BufferState SpeculateBufferStateTo(RGPass* readPass);
        static RHI::TextureState SpeculateTextureStateFrom(RGPass* writePass);
        static RHI::TextureState SpeculateTextureStateTo(RGPass* readPass);
        static void TransitionSingleBuffer(RHI::CommandEncoder* encoder, RGBuffer* buffer, RGPass* writePass, RGPass* readPass);
        static void TransitionSingleBufferView(RHI::CommandEncoder* encoder, RGBufferView* bufferView, RGPass* writePass, RGPass* readPass);
        static void TransitionSingleTexture(RHI::CommandEncoder* encoder, RGTexture* texture, RGPass* writePass, RGPass* readPass);
        static void TransitionSingleTextureView(RHI::CommandEncoder* encoder, RGTextureView* textureView, RGPass* writePass, RGPass* readPass);
        static void TransitionSingleResource(RHI::CommandEncoder* encoder, RGResource* resource, RGPass* writePass, RGPass* readPass);
        static void ExecuteCopyPass(RHI::CommandEncoder* encoder, RGCopyPass* copyPass);
        static void ExecuteComputePass(RHI::CommandEncoder* encoder, RGComputePass* computePass);
        static void ExecuteRasterPass(RHI::CommandEncoder* encoder, RGRasterPass* rasterPass);

        void TransitionResources(RHI::CommandEncoder* encoder, RGPass* readPass);

        RHI::Device& device;
        std::vector<std::unique_ptr<RGResource>> resources;
        std::vector<std::unique_ptr<RGPass>> passes;
        std::unordered_map<RGResource*, RGPass*> writes;
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
            pass.reads.emplace_back(resource);
        }

        template <typename R>
        void Write(R* resource)
        {
            pass.writes.emplace_back(resource);
        }

    protected:
        RenderGraph& graph;
        RGPass& pass;
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
