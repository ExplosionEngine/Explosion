//
// Created by johnk on 2023/11/28.
//

#pragma once

#include <unordered_map>
#include <functional>
#include <optional>

#include <Common/Memory.h>
#include <RHI/RHI.h>
#include <Rendering/ResourcePool.h>
#include <Rendering/RenderingCache.h>

namespace Rendering {
    enum class RGResType : uint8_t {
        buffer,
        texture,
        max
    };

    enum class RGResViewType : uint8_t {
        bufferView,
        textureView,
        max
    };

    enum class RGPassType : uint8_t {
        copy,
        compute,
        raster,
        max
    };

    enum class RGQueueType : uint8_t {
        main,
        asyncCompute,
        asyncCopy,
        max
    };

    using RGBufferDesc = RHI::BufferCreateInfo;
    using RGTextureDesc = RHI::TextureCreateInfo;

    class RGResource {
    public:
        virtual ~RGResource();

        RGResType Type() const;
        void MaskAsUsed();

    protected:
        friend class RGBuilder;
        friend class RGPass;

        explicit RGResource(RGResType inType);

        RGResType type;
        bool forceUsed;
        bool imported;
    };

    class RGBuffer final : public RGResource {
    public:
        ~RGBuffer() override;

        const RGBufferDesc& GetDesc() const;

    private:
        friend class RGBuilder;
        friend class RGPass;

        explicit RGBuffer(RGBufferDesc inDesc);
        explicit RGBuffer(RHI::Buffer* inImportedBuffer);

        RGBufferDesc desc;
        RHI::Buffer* rhiHandleImported;
    };

    class RGTexture final : public RGResource {
    public:
        ~RGTexture() override;

        const RGTextureDesc& GetDesc() const;

    private:
        friend class RGBuilder;
        friend class RGPass;

        explicit RGTexture(RGTextureDesc inDesc);
        explicit RGTexture(RHI::Texture* inImportedTexture);

        RGTextureDesc desc;
        RHI::Texture* rhiHandleImported;
    };

    using RGResourceRef = RGResource*;
    using RGBufferRef = RGBuffer*;
    using RGTextureRef = RGTexture*;

    using RGBufferViewDesc = RHI::BufferViewCreateInfo;
    using RGTextureViewDesc = RHI::TextureViewCreateInfo;

    class RGResourceView {
    public:
        virtual ~RGResourceView();

        RGResViewType Type() const;

        virtual RGResourceRef GetResource() = 0;

    protected:
        explicit RGResourceView(RGResViewType inType);

        RGResViewType type;
    };

    class RGBufferView final : public RGResourceView {
    public:
        ~RGBufferView() override;

        const RGBufferViewDesc& GetDesc() const;
        RGBufferRef GetBuffer() const;
        RGResourceRef GetResource() override;

    private:
        friend class RGBuilder;

        RGBufferView(RGBufferRef inBuffer, const RGBufferViewDesc& inDesc);

        RGBufferRef buffer;
        RGBufferViewDesc desc;
    };

    class RGTextureView final : public RGResourceView {
    public:
        ~RGTextureView() override;

        const RGTextureViewDesc& GetDesc() const;
        RGTextureRef GetTexture() const;

        RGResourceRef GetResource() override;

    private:
        friend class RGBuilder;

        RGTextureView(RGTextureRef inTexture, const RGTextureViewDesc& inDesc);

        RGTextureRef texture;
        RGTextureViewDesc desc;
    };

    using RGResourceViewRef = RGResourceView*;
    using RGBufferViewRef = RGBufferView*;
    using RGTextureViewRef = RGTextureView*;

    struct RGColorAttachment : RHI::ColorAttachmentBase<RGColorAttachment> {
        RGTextureViewRef view;
        // TODO TextureView* resolve;
    };

    struct RGDepthStencilAttachment : RHI::DepthStencilAttachmentBase<RGDepthStencilAttachment> {
        RGTextureViewRef view;
    };

    struct RGRasterPassDesc {
        std::vector<RGColorAttachment> colorAttachments;
        std::optional<RGDepthStencilAttachment> depthStencilAttachment;
    };

    struct RGCopyPassDesc {
        std::vector<RGResourceRef> copySrcs;
        std::vector<RGResourceRef> copyDsts;
    };

    struct RGBindItemDesc {
        RHI::BindingType type;
        std::variant<RGBufferViewRef, RGTextureViewRef, RHI::Sampler*> view;
    };

    struct RGBindGroupDesc {
        BindGroupLayout* layout;
        std::unordered_map<std::string, RGBindItemDesc> items;

        static RGBindGroupDesc Create(BindGroupLayout* inLayout);
        RGBindGroupDesc& Sampler(std::string inName, RHI::Sampler* inSampler);
        RGBindGroupDesc& UniformBuffer(std::string inName, RGBufferViewRef bufferView);
        RGBindGroupDesc& StorageBuffer(std::string inName, RGBufferViewRef bufferView);
        RGBindGroupDesc& Texture(std::string inName, RGTextureViewRef textureView);
        RGBindGroupDesc& StorageTexture(std::string inName, RGTextureViewRef textureView);
    };

    class RGBindGroup {
    public:
        ~RGBindGroup();

        const RGBindGroupDesc& GetDesc() const;

    private:
        friend class RGBuilder;

        explicit RGBindGroup(RGBindGroupDesc inDesc);

        RGBindGroupDesc desc;
    };

    using RGBindGroupRef = RGBindGroup*;

    class RGPass {
    public:
        virtual ~RGPass();

    protected:
        friend class RGBuilder;

        RGPass(std::string inName, RGPassType inType);

        std::string name;
        RGPassType type;
    };

    using RGPassRef = RGPass*;

    using RGCopyPassExecuteFunc = std::function<void(const RGBuilder&, RHI::CopyPassCommandRecorder&)>;
    using RGComputePassExecuteFunc = std::function<void(const RGBuilder&, RHI::ComputePassCommandRecorder&)>;
    using RGRasterPassExecuteFunc = std::function<void(const RGBuilder&, RHI::RasterPassCommandRecorder&)>;

    class RGCopyPass final : public RGPass {
    public:
        ~RGCopyPass() override;

    private:
        friend class RGBuilder;

        RGCopyPass(std::string inName, RGCopyPassDesc inPassDesc, RGCopyPassExecuteFunc inFunc);

        RGCopyPassDesc passDesc;
        RGCopyPassExecuteFunc func;
    };

    class RGComputePass final : public RGPass {
    public:
        ~RGComputePass() override;

    private:
        friend class RGBuilder;

        RGComputePass(std::string inName, std::vector<RGBindGroupRef> inBindGroups, RGComputePassExecuteFunc inFunc);

        RGComputePassExecuteFunc func;
        std::vector<RGBindGroupRef> bindGroups;
    };

    class RGRasterPass final : public RGPass {
    public:
        ~RGRasterPass() override;

    private:
        friend class RGBuilder;

        RGRasterPass(std::string inName, RGRasterPassDesc inPassDesc, std::vector<RGBindGroupRef> inBindGroupds, RGRasterPassExecuteFunc inFunc);

        RGRasterPassDesc passDesc;
        RGRasterPassExecuteFunc func;
        std::vector<RGBindGroupRef> bindGroups;
    };

    class RGBuilder {
    public:
        NonCopyable(RGBuilder);
        explicit RGBuilder(RHI::Device& inDevice);
        ~RGBuilder();

        // setup
        RGBufferRef CreateBuffer(const RGBufferDesc& inDesc);
        RGTextureRef CreateTexture(const RGTextureDesc& inDesc);
        RGBufferViewRef CreateBufferView(RGBufferRef inBuffer, const RGBufferViewDesc& inDesc);
        RGTextureViewRef CreateTextureView(RGTextureRef inTexture, const RGTextureViewDesc& inDesc);
        RGBufferRef ImportBuffer(RHI::Buffer* inBuffer);
        RGTextureRef ImportTexture(RHI::Texture* inTexture);
        RGBindGroupRef AllocateBindGroup(const RGBindGroupDesc& inDesc);
        void AddCopyPass(const std::string& inName, const RGCopyPassDesc& inPassDesc, const RGCopyPassExecuteFunc& inFunc);
        void AddComputePass(const std::string& inName, const std::vector<RGBindGroupRef>& inBindGroups, const RGComputePassExecuteFunc& inFunc);
        void AddRasterPass(const std::string& inName, const RGRasterPassDesc& inPassDesc, const std::vector<RGBindGroupRef>& inBindGroupds, const RGRasterPassExecuteFunc& inFunc);
        void AddSyncPoint();
        void Execute(const RHI::Fence& inFence);

        // execute
        RHI::Buffer* GetRHI(RGBufferRef inBuffer) const;
        RHI::Texture* GetRHI(RGTextureRef inTexture) const;
        RHI::BufferView* GetRHI(RGBufferViewRef inBufferView) const;
        RHI::TextureView* GetRHI(RGTextureViewRef inTextureView) const;
        RHI::BindGroup* GetRHI(RGBindGroupRef inBindGroup) const;

    private:
        void Compile();
        void ExecuteInternal(const RHI::Fence& inFence);

        void CompilePassReadWrites();
        void PerformSyncCheck();
        void PerformCull();
        void DevirtualizeResources();
        void DevirtualizeBindGroups();

        bool executed;
        RHI::Device& device;
        std::vector<Common::UniqueRef<RGResource>> resources;
        std::vector<Common::UniqueRef<RGResourceView>> views;
        std::vector<Common::UniqueRef<RGBindGroup>> bindGroups;
        std::vector<Common::UniqueRef<RGPass>> passes;
        std::unordered_map<RGQueueType, std::vector<RGPassRef>> queuePassesTemp;
        std::vector<std::unordered_map<RGQueueType, std::vector<RGPassRef>>> queuePassesVec;

        // execute context
        std::unordered_map<RGResourceRef, uint32_t> resourceReadCounts;
        std::unordered_map<RGPassRef, std::unordered_set<RGResourceRef>> passReadsMap;
        std::unordered_map<RGPassRef, std::unordered_set<RGResourceRef>> passWritesMap;
        std::unordered_set<RGResourceRef> culledResources;
        std::unordered_set<RGPassRef> culledPasses;
        std::unordered_map<RGResourceRef, std::variant<PooledBufferRef, PooledTextureRef>> devirtualizedResources;
        std::unordered_map<RGResourceViewRef, std::variant<RHI::BufferView*, RHI::TextureView*>> devirtualizedResourceViews;
        std::unordered_map<RGBindGroupRef, Common::UniqueRef<RHI::BindGroup>> devirtualizedBindGroups;
    };
}
