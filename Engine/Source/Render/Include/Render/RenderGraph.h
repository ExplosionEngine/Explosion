//
// Created by johnk on 2023/2/10.
//

#pragma once

#include <string>
#include <vector>
#include <functional>

#include <RHI/RHI.h>

namespace Render {
    class RGResource;
    class RGPass;
    class RGPassBuilder;

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

    private:
        friend class RenderGraph;

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
        virtual void Setup(RGPassBuilder& builder) = 0;

    private:
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
        virtual void Execute(RHI::CommandEncoder& encoder) = 0;
    };

    class RGComputePass : public RGPass {
    public:
        ~RGComputePass() override;

    protected:
        explicit RGComputePass(std::string inName);

        RGPassType GetType() override;
        virtual void Execute(RHI::ComputePassCommandEncoder& encoder) = 0;
    };

    class RGRasterPass : public RGPass {
    public:
        ~RGRasterPass() override;

    protected:
        explicit RGRasterPass(std::string inName);

        RGPassType GetType() override;
        virtual void Execute(RHI::GraphicsPassCommandEncoder& encoder) = 0;
    };

    using RGCopyPassExecuteFunc = std::function<void(RHI::CommandEncoder&)>;
    using RGCopyPassSetupFunc = std::function<RGCopyPassExecuteFunc(RGPassBuilder&)>;
    using RGComputePassExecuteFunc = std::function<void(RHI::ComputePassCommandEncoder&)>;
    using RGComputePassSetupFunc = std::function<RGComputePassExecuteFunc(RGPassBuilder&)>;
    using RGRasterPassExecuteFunc = std::function<void(RHI::GraphicsPassCommandEncoder&)>;
    using RGRasterPassSetupFunc = std::function<RGRasterPassExecuteFunc(RGPassBuilder&)>;

    class RGFuncCopyPass : public RGCopyPass {
    public:
        RGFuncCopyPass(std::string inName, RGCopyPassSetupFunc inSetupFunc);
        ~RGFuncCopyPass() override;

        void Setup(RGPassBuilder& builder) override;
        void Execute(RHI::CommandEncoder& encoder) override;

    private:
        RGCopyPassSetupFunc setupFunc;
        RGCopyPassExecuteFunc executeFunc;
    };

    class RGFuncComputePass : public RGComputePass {
    public:
        RGFuncComputePass(std::string inName, RGComputePassSetupFunc inSetupFunc);
        ~RGFuncComputePass() override;

        void Setup(RGPassBuilder& builder) override;
        void Execute(RHI::ComputePassCommandEncoder &encoder) override;

    private:
        RGComputePassSetupFunc setupFunc;
        RGComputePassExecuteFunc executeFunc;
    };

    class RGFuncRasterPass : public RGRasterPass {
    public:
        RGFuncRasterPass(std::string inName, RGRasterPassSetupFunc inSetupFunc);
        ~RGFuncRasterPass() override;

        void Setup(RGPassBuilder& builder) override;
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

        void Compile();
        void Setup();
        void Execute();

    private:
        friend class RGPassBuilder;

        RHI::Device& device;
        std::vector<std::unique_ptr<RGResource>> resources;
        std::vector<std::unique_ptr<RGPass>> passes;
    };

    class RGPassBuilder {
    public:
        RGPassBuilder(RenderGraph& inGraph, RGPass& inPass);
        ~RGPassBuilder();

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

    private:
        RenderGraph& graph;
        RGPass& pass;
    };
}
