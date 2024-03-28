//
// Created by johnk on 2023/11/28.
//

#pragma once

#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <variant>
#include <optional>

#include <Common/Memory.h>
#include <Common/Debug.h>
#include <RHI/RHI.h>
#include <Rendering/ResourcePool.h>
#include <Rendering/RenderingCache.h>

namespace Rendering {
    class RGAsyncInfo;
    class RGFencePack;
}

namespace Rendering::Internal {
    class CommandBuffersGuard {
    public:
        struct Context {
            RHI::CommandBuffer* mainCmdBuffer;
            RHI::CommandBuffer* asyncCopyCmdBuffer;
            RHI::CommandBuffer* asyncComputeCmdBuffer;
        };

        CommandBuffersGuard(RHI::Device& inDevice, const RGAsyncInfo& inAsyncInfo, const RGFencePack& inFencePack, std::function<void(const Context&)>&& inAction);
        ~CommandBuffersGuard();

    private:
        RHI::Device& device;
        const RGFencePack& fencePack;
        bool useAsyncCopy;
        bool useAsyncCompute;
        Common::UniqueRef<RHI::CommandBuffer> mainCmdBuffer;
        Common::UniqueRef<RHI::CommandBuffer> asyncCopyCmdBuffer;
        Common::UniqueRef<RHI::CommandBuffer> asyncComputeCmdBuffer;
    };
}

namespace Rendering {
    enum class RGResType {
        buffer,
        texture,
        max
    };

    enum class RGResViewType {
        bufferView,
        textureView,
        max
    };

    enum class RGPassType {
        copy,
        compute,
        raster,
        max
    };

    struct RGBufferDesc : public RHI::BufferCreateInfo {
        static RGBufferDesc Create();
        static RGBufferDesc Create(const RHI::BufferCreateInfo& rhiDesc);
        RGBufferDesc& Size(uint32_t inSize);
        RGBufferDesc& Usages(RHI::BufferUsageFlags inUsages);
        RGBufferDesc& InitialState(RHI::BufferState inState);
        RGBufferDesc& DebugName(const std::string& inName);
    };

    struct RGTextureDesc  : public RHI::TextureCreateInfo {
        static RGTextureDesc Create();
        static RGTextureDesc Create(const RHI::TextureCreateInfo& rhiDesc);
        RGTextureDesc& Dimension(RHI::TextureDimension inDimension);
        RGTextureDesc& Extent(const Common::UVec3& inExtent);
        RGTextureDesc& Format(RHI::PixelFormat inFormat);
        RGTextureDesc& Usages(RHI::TextureUsageFlags inUsages);
        RGTextureDesc& MipLevels(uint8_t inMipLevels);
        RGTextureDesc& Samples(uint8_t inSamples);
        RGTextureDesc& InitialState(RHI::TextureState inState);
        RGTextureDesc& DebugName(const std::string& inName);
    };

    class RGResource {
    public:
        virtual ~RGResource();

        RGResType Type() const;
        void MaskAsUsed();

    protected:
        friend class RGBuilder;
        friend class RGPass;

        explicit RGResource(RGResType inType);
        bool IsForceUsed() const;
        void IncRefCountAndUpdateResource(RHI::Device& device);
        void DecRefAndUpdateResource();

        virtual void Devirtualize(RHI::Device& device) = 0;
        virtual void UndoDevirtualize() = 0;

        RGResType type;
        bool forceUsed;
        bool imported;
        bool devirtualized;
        uint32_t refCount;
    };

    class RGBuffer : public RGResource {
    public:
        ~RGBuffer() override;

        const RGBufferDesc& GetDesc() const;
        RHI::Buffer* GetRHI() const;

    private:
        friend class RGBuilder;
        friend class RGPass;

        explicit RGBuffer(RGBufferDesc inDesc);
        explicit RGBuffer(RHI::Buffer* inImportedBuffer);
        void Transition(RHI::CommandCommandEncoder& commandEncoder, RHI::BufferState transitionTo);

        void Devirtualize(RHI::Device& device) override;
        void UndoDevirtualize() override;

        RGBufferDesc desc;
        RHI::Buffer* rhiHandle;
        PooledBufferRef pooledBuffer;
        RHI::BufferState currentState;
    };

    class RGTexture : public RGResource {
    public:
        ~RGTexture() override;

        const RGTextureDesc& GetDesc() const;
        RHI::Texture* GetRHI() const;

    private:
        friend class RGBuilder;
        friend class RGPass;

        explicit RGTexture(RGTextureDesc inDesc);
        explicit RGTexture(RHI::Texture* inImportedTexture);
        void Transition(RHI::CommandCommandEncoder& commandEncoder, RHI::TextureState transitionTo);

        void Devirtualize(RHI::Device& device) override;
        void UndoDevirtualize() override;

        RGTextureDesc desc;
        RHI::Texture* rhiHandle;
        PooledTextureRef pooledTexture;
        RHI::TextureState currentState;
    };

    using RGResourceRef = RGResource*;
    using RGBufferRef = RGBuffer*;
    using RGTextureRef = RGTexture*;

    struct RGBufferViewDesc : public RHI::BufferViewCreateInfoBase<RGBufferViewDesc> {
        static RGBufferViewDesc CreateForUniform();
        static RGBufferViewDesc CreateForStorage();
        static RGBufferViewDesc Create(const RHI::BufferViewCreateInfo& rhiDesc);
    };

    struct RGTextureViewDesc  : public RHI::TextureViewCreateInfo {
        static RGTextureViewDesc CreateForTexture();
        static RGTextureViewDesc CreateForStorageTexture();
        static RGTextureViewDesc CreateForColorAttachment();
        static RGTextureViewDesc CreateForDepthStencilAttachment();
        static RGTextureViewDesc Create(const RHI::TextureViewCreateInfo& rhiDesc);
        RGTextureViewDesc& Dimension(RHI::TextureViewDimension inDimension);
        RGTextureViewDesc& Aspect(RHI::TextureAspect inAspect);
        RGTextureViewDesc& BaseMipLevel(uint8_t inBaseMipLevel);
        RGTextureViewDesc& MipLevelNum(uint8_t inMipLevelNum);
        RGTextureViewDesc& BaseArrayLayer(uint8_t inBaseArrayLayer);
        RGTextureViewDesc& ArrayLayerNum(uint8_t inArrayLayerNum);
    };

    class RGResourceView {
    public:
        virtual ~RGResourceView();

        RGResViewType Type() const;
        virtual RGResourceRef GetResource() = 0;

    protected:
        explicit RGResourceView(RGResViewType inType);

        RGResViewType type;
        bool devirtualized;
    };

    class RGBufferView : public RGResourceView {
    public:
        ~RGBufferView() override;

        const RGBufferViewDesc& GetDesc() const;
        RGBufferRef GetBuffer() const;
        RHI::BufferView* GetRHI() const;
        RGResourceRef GetResource() override;

    private:
        friend class RGBuilder;

        RGBufferView(RGBufferRef inBuffer, const RGBufferViewDesc& inDesc);

        RGBufferRef buffer;
        RGBufferViewDesc desc;
        RHI::BufferView* rhiHandle;
    };

    class RGTextureView : public RGResourceView {
    public:
        ~RGTextureView() override;

        const RGTextureViewDesc& GetDesc() const;
        RGTextureRef GetTexture() const;
        RHI::TextureView* GetRHI() const;
        RGResourceRef GetResource() override;

    private:
        friend class RGBuilder;

        RGTextureView(RGTextureRef inTexture, const RGTextureViewDesc& inDesc);

        RGTextureRef texture;
        RGTextureViewDesc desc;
        RHI::TextureView* rhiHandle;
    };

    using RGResourceViewRef = RGResourceView*;
    using RGBufferViewRef = RGBufferView*;
    using RGTextureViewRef = RGTextureView*;

    struct RGColorAttachment : public RHI::GraphicsPassColorAttachmentBase<RGColorAttachment> {
        RGTextureViewRef view;
        // TODO TextureView* resolve;
    };

    struct RGDepthStencilAttachment : public RHI::GraphicsPassDepthStencilAttachmentBase<RGDepthStencilAttachment> {
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
        union {
            RHI::Sampler* sampler;
            RGTextureViewRef textureView;
            RGBufferViewRef bufferView;
        };
    };

    struct RGBindGroupDesc {
        Rendering::BindGroupLayout* layout;
        std::unordered_map<std::string, RGBindItemDesc> items;

        static RGBindGroupDesc Create(Rendering::BindGroupLayout* inLayout);
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
        RHI::BindGroup* GetRHI() const;

    private:
        friend class RGBuilder;

        explicit RGBindGroup(RGBindGroupDesc inDesc);
        void Devirtualize(RHI::Device& inDevice);
        void UndoDevirtualize();

        bool devirtualized;
        RGBindGroupDesc desc;
        RHI::BindGroup* rhiHandle;
    };

    using RGBindGroupRef = RGBindGroup*;

    struct RGAsyncInfo {
        bool hasAsyncCopy;
        bool hasAsyncCompute;

        RGAsyncInfo();
    };

    struct RGResourcesStates {
        std::unordered_map<RGBufferRef, RHI::BufferState> buffer;
        std::unordered_map<RGTextureRef, RHI::TextureState> texture;
    };

    class RGPass {
    public:
        virtual ~RGPass();

    protected:
        friend class RGBuilder;

        RGPass(std::string inName, RGPassType inType);
        void SaveBufferTransitionInfo(RGBufferRef buffer, RHI::BufferState state);
        void SaveTextureTransitionInfo(RGTextureRef texture, RHI::TextureState state);
        void CompileForBindGroups(const std::vector<RGBindGroupRef>& bindGroups);
        void DevirtualizeResources(RHI::Device& device);
        void TransitionResources(RHI::CommandCommandEncoder* commandEncoder);
        void FinalizeResources();

        virtual void Compile(RGAsyncInfo& outAsyncInfo) = 0;
        virtual void Execute(RHI::Device& device, const Internal::CommandBuffersGuard::Context& cmdBuffers) = 0;

        std::string name;
        RGPassType type;
        std::unordered_set<RGResourceRef> reads;
        RGResourcesStates transitionInfos;
    };

    using RGCopyPassExecuteFunc = std::function<void(RHI::CopyPassCommandEncoder&)>;
    using RGComputePassExecuteFunc = std::function<void(RHI::ComputePassCommandEncoder&)>;
    using RGRasterPassExecuteFunc = std::function<void(RHI::GraphicsPassCommandEncoder&)>;

    class RGCopyPass : public RGPass {
    public:
        ~RGCopyPass() override;

    private:
        friend class RGBuilder;

        RGCopyPass(std::string inName, RGCopyPassDesc inPassDesc, RGCopyPassExecuteFunc inFunc, bool inAsyncCopy = false);
        void CompileForCopyPassDesc();
        void Compile(RGAsyncInfo& outAsyncInfo) override;
        void Execute(RHI::Device& device, const Internal::CommandBuffersGuard::Context& cmdBuffers) override;

        bool asyncCopy;
        RGCopyPassDesc passDesc;
        RGCopyPassExecuteFunc func;
    };

    class RGComputePass : public RGPass {
    public:
        ~RGComputePass() override;

    private:
        friend class RGBuilder;

        RGComputePass(std::string inName, std::vector<RGBindGroupRef> inBindGroups, RGComputePassExecuteFunc inFunc, bool inAsyncCompute = false);
        void Compile(RGAsyncInfo& outAsyncInfo) override;
        void Execute(RHI::Device& device, const Internal::CommandBuffersGuard::Context& cmdBuffers) override;

        bool asyncCompute;
        RGComputePassExecuteFunc func;
        std::vector<RGBindGroupRef> bindGroups;
    };

    class RGRasterPass : public RGPass {
    public:
        ~RGRasterPass() override;

    private:
        friend class RGBuilder;

        RGRasterPass(std::string inName, RGRasterPassDesc inPassDesc, std::vector<RGBindGroupRef> inBindGroupds, RGRasterPassExecuteFunc inFunc);
        void CompileForRasterPassDesc();
        void Compile(RGAsyncInfo& outAsyncInfo) override;
        void Execute(RHI::Device& device, const Internal::CommandBuffersGuard::Context& cmdBuffers) override;

        RGRasterPassDesc passDesc;
        RGRasterPassExecuteFunc func;
        std::vector<RGBindGroupRef> bindGroups;
    };

    struct RGFencePack {
        RHI::Fence* mainFence;
        RHI::Fence* asyncComputeFence;
        RHI::Fence* asyncCopyFence;

        RGFencePack();
        ~RGFencePack();
        RGFencePack(RHI::Fence* inMainFence, RHI::Fence* inAsyncComputeFence = nullptr, RHI::Fence* inAsyncCopyFence = nullptr);
    };

    class RGBuilder {
    public:
        NonCopyable(RGBuilder);
        explicit RGBuilder(RHI::Device& inDevice);
        ~RGBuilder();

        RGBufferRef CreateBuffer(const RGBufferDesc& inDesc);
        RGTextureRef CreateTexture(const RGTextureDesc& inDesc);
        RGBufferViewRef CreateBufferView(RGBufferRef inBuffer, const RGBufferViewDesc& inDesc);
        RGTextureViewRef CreateTextureView(RGTextureRef inTexture, const RGTextureViewDesc& inDesc);
        RGBufferRef ImportBuffer(RHI::Buffer* inBuffer);
        RGTextureRef ImportTexture(RHI::Texture* inTexture);
        RGBindGroupRef AllocateBindGroup(const RGBindGroupDesc& inDesc);
        void AddCopyPass(const std::string& inName, const RGCopyPassDesc& inPassDesc, const RGCopyPassExecuteFunc& inFunc, bool inAsyncCopy = false);
        void AddComputePass(const std::string& inName, const std::vector<RGBindGroupRef>& inBindGroups, const RGComputePassExecuteFunc& inFunc, bool inAsyncCompute = false);
        void AddRasterPass(const std::string& inName, const RGRasterPassDesc& inPassDesc, const std::vector<RGBindGroupRef>& inBindGroupds, const RGRasterPassExecuteFunc& inFunc);
        void Execute(const RGFencePack& inFencePack);

    private:
        void Compile();
        void ExecuteInternal(const RGFencePack& inFencePack);

        bool executed;
        RHI::Device& device;
        std::vector<Common::UniqueRef<RGResource>> resources;
        std::vector<Common::UniqueRef<RGResourceView>> views;
        std::vector<Common::UniqueRef<RGBindGroup>> bindGroups;
        std::vector<Common::UniqueRef<RGPass>> passes;

        // execute context
        RGAsyncInfo asyncInfo;
    };
}
