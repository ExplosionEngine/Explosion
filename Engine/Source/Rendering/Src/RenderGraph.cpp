//
// Created by johnk on 2023/11/28.
//

#include <ranges>

#include <Rendering/RenderGraph.h>
#include <Common/Container.h>

namespace Rendering::Internal {
    static bool IsBufferStateRead(RHI::BufferState state)
    {
        return state == RHI::BufferState::copySrc
            || state == RHI::BufferState::shaderReadOnly;
    }

    static bool IsBufferStateWrite(RHI::BufferState state)
    {
        return state == RHI::BufferState::copyDst
            || state == RHI::BufferState::storage;
    }

    static bool IsTextureStateRead(RHI::TextureState state)
    {
        return state == RHI::TextureState::copySrc
            || state == RHI::TextureState::shaderReadOnly
            || state == RHI::TextureState::depthStencilReadonly;
    }

    static bool IsTextureStateWrite(RHI::TextureState state)
    {
        return state == RHI::TextureState::copyDst
            || state == RHI::TextureState::shaderReadOnly
            || state == RHI::TextureState::renderTarget
            || state == RHI::TextureState::storage
            || state == RHI::TextureState::depthStencilWrite;
    }

    static std::optional<RHI::DepthStencilAttachment> GetRasterPassDepthStencilAttachment(const RGRasterPassDesc& desc)
    {
        static_assert(std::is_base_of_v<RHI::DepthStencilAttachmentBase<RGDepthStencilAttachment>, RGDepthStencilAttachment>);

        std::optional<RHI::DepthStencilAttachment> result;
        if (desc.depthStencilAttachment.has_value()) {
            result = RHI::DepthStencilAttachment {};
            memcpy(&result.value(), &desc.depthStencilAttachment.value(), sizeof(RHI::DepthStencilAttachmentBase<RGDepthStencilAttachment>));
            result->view = desc.depthStencilAttachment->view->GetRHI();
        }
        return result;
    }

    static std::vector<RHI::ColorAttachment> GetRasterPassColorAttachments(const RGRasterPassDesc& desc)
    {
        static_assert(std::is_base_of_v<RHI::ColorAttachmentBase<RGColorAttachment>, RGColorAttachment>);

        std::vector<RHI::ColorAttachment> result;
        result.reserve(desc.colorAttachments.size());

        for (const auto& colorAttachment : desc.colorAttachments) {
            RHI::ColorAttachment back;
            memcpy(&back, &colorAttachment, sizeof(RHI::ColorAttachmentBase<RGColorAttachment>));
            back.view = colorAttachment.view->GetRHI();
            result.emplace_back(std::move(back));
        }
        return result;
    }

    CommandBuffersGuard::CommandBuffersGuard(RHI::Device& inDevice, const RGAsyncInfo& inAsyncInfo, const RGFencePack& inFencePack, std::function<void(const Context&)>&& inAction)
        : device(inDevice)
        , fencePack(inFencePack)
    {
        const bool allowAsyncCopy = device.GetQueueNum(RHI::QueueType::transfer) > 1;
        const bool allowAsyncCompute = device.GetQueueNum(RHI::QueueType::compute) > 1;
        useAsyncCopy = inAsyncInfo.hasAsyncCopy && allowAsyncCopy;
        useAsyncCompute = inAsyncInfo.hasAsyncCompute && allowAsyncCompute;

        mainCmdBuffer = device.CreateCommandBuffer();
        if (useAsyncCopy) {
            asyncCopyCmdBuffer = device.CreateCommandBuffer();
        }
        if (useAsyncCompute) {
            asyncComputeCmdBuffer = device.CreateCommandBuffer();
        }

        Context context;
        context.mainCmdBuffer = mainCmdBuffer.Get();
        context.asyncCopyCmdBuffer = useAsyncCopy ? asyncCopyCmdBuffer.Get() : mainCmdBuffer.Get();
        context.asyncComputeCmdBuffer = useAsyncCompute ? asyncComputeCmdBuffer.Get() : mainCmdBuffer.Get();
        inAction(context);
    }

    CommandBuffersGuard::~CommandBuffersGuard()
    {
        if (fencePack.mainFence) {
            fencePack.mainFence->Reset();
        }
        if (fencePack.asyncCopyFence) {
            fencePack.asyncCopyFence->Reset();
        }
        if (fencePack.asyncComputeFence) {
            fencePack.asyncComputeFence->Reset();
        }

        {
            RHI::QueueSubmitInfo submitInfo {};
            submitInfo.signalFence = fencePack.mainFence;
            device.GetQueue(RHI::QueueType::graphics, 0)->Submit(mainCmdBuffer.Get(), submitInfo);
        }
        if (useAsyncCopy) {
            RHI::QueueSubmitInfo submitInfo {};
            submitInfo.signalFence = fencePack.asyncCopyFence;
            device.GetQueue(RHI::QueueType::transfer, 1)->Submit(asyncCopyCmdBuffer.Get(), submitInfo);
        }
        if (useAsyncCompute) {
            RHI::QueueSubmitInfo submitInfo {};
            submitInfo.signalFence = fencePack.asyncComputeFence;
            device.GetQueue(RHI::QueueType::compute, 1)->Submit(asyncComputeCmdBuffer.Get(), submitInfo);
        }
    }
}

namespace Rendering {
    RGBufferDesc RGBufferDesc::Create()
    {
        return RGBufferDesc {};
    }

    RGBufferDesc RGBufferDesc::Create(const RHI::BufferCreateInfo& rhiDesc)
    {
        RGBufferDesc result {};
        result.size = rhiDesc.size;
        result.usages = rhiDesc.usages;
        result.initialState = rhiDesc.initialState;
        result.debugName = rhiDesc.debugName;
        return result;
    }

    RGBufferDesc& RGBufferDesc::Size(uint32_t inSize)
    {
        size = inSize;
        return *this;
    }

    RGBufferDesc& RGBufferDesc::Usages(RHI::BufferUsageFlags inUsages)
    {
        usages = inUsages;
        return *this;
    }

    RGBufferDesc& RGBufferDesc::InitialState(RHI::BufferState inState)
    {
        initialState = inState;
        return *this;
    }

    RGBufferDesc& RGBufferDesc::DebugName(const std::string& inName)
    {
        debugName = inName;
        return *this;
    }

    RGTextureDesc RGTextureDesc::Create()
    {
        return RGTextureDesc {};
    }

    RGTextureDesc RGTextureDesc::Create(const RHI::TextureCreateInfo& rhiDesc)
    {
        RGTextureDesc result {};
        result.dimension = rhiDesc.dimension;
        result.extent = rhiDesc.extent;
        result.format = rhiDesc.format;
        result.usages = rhiDesc.usages;
        result.mipLevels = rhiDesc.mipLevels;
        result.samples = rhiDesc.samples;
        result.initialState = rhiDesc.initialState;
        result.debugName = rhiDesc.debugName;
        return result;
    }

    RGTextureDesc& RGTextureDesc::Dimension(RHI::TextureDimension inDimension)
    {
        dimension = inDimension;
        return *this;
    }

    RGTextureDesc& RGTextureDesc::Extent(const Common::UVec3& inExtent)
    {
        extent = inExtent;
        return *this;
    }

    RGTextureDesc& RGTextureDesc::Format(RHI::PixelFormat inFormat)
    {
        format = inFormat;
        return *this;
    }

    RGTextureDesc& RGTextureDesc::Usages(RHI::TextureUsageFlags inUsages)
    {
        usages = inUsages;
        return *this;
    }

    RGTextureDesc& RGTextureDesc::MipLevels(uint8_t inMipLevels)
    {
        mipLevels = inMipLevels;
        return *this;
    }

    RGTextureDesc& RGTextureDesc::Samples(uint8_t inSamples)
    {
        samples = inSamples;
        return *this;
    }

    RGTextureDesc& RGTextureDesc::InitialState(RHI::TextureState inState)
    {
        initialState = inState;
        return *this;
    }

    RGTextureDesc& RGTextureDesc::DebugName(const std::string& inName)
    {
        debugName = inName;
        return *this;
    }

    RGResource::RGResource(RGResType inType)
        : type(inType)
        , forceUsed(false)
        , imported(false)
        , devirtualized(false)
        , refCount(0)
    {
    }

    RGResource::~RGResource() = default;

    RGResType RGResource::Type() const
    {
        return type;
    }

    void RGResource::MaskAsUsed()
    {
        forceUsed = true;
    }

    bool RGResource::IsForceUsed() const
    {
        return forceUsed;
    }

    void RGResource::IncRefCountAndUpdateResource(RHI::Device& device)
    {
        if (refCount == 0) {
            Devirtualize(device);
        }
        refCount++;
    }

    void RGResource::DecRefAndUpdateResource()
    {
        refCount--;
        if (refCount == 0) {
            UndoDevirtualize();
        }
    }

    RGBuffer::RGBuffer(RGBufferDesc inDesc)
        : RGResource(RGResType::buffer)
        , desc(std::move(inDesc))
        , rhiHandle(nullptr)
        , pooledBuffer()
        , currentState(desc.initialState)
    {
    }

    RGBuffer::RGBuffer(RHI::Buffer* inImportedBuffer)
        : RGResource(RGResType::buffer)
        , desc(RGBufferDesc::Create(inImportedBuffer->GetCreateInfo()))
        , rhiHandle(inImportedBuffer)
        , pooledBuffer()
        , currentState(desc.initialState)
    {
        imported = true;
    }

    RGBuffer::~RGBuffer() = default;

    void RGBuffer::Transition(RHI::CommandCommandRecorder& commandRecorder, RHI::BufferState transitionTo)
    {
        commandRecorder.ResourceBarrier(RHI::Barrier::Transition(GetRHI(), currentState, transitionTo));
        currentState = transitionTo;
    }

    const RGBufferDesc& RGBuffer::GetDesc() const
    {
        return desc;
    }

    RHI::Buffer* RGBuffer::GetRHI() const
    {
        Assert(devirtualized);
        return rhiHandle;
    }

    void RGBuffer::Devirtualize(RHI::Device& device)
    {
        Assert(!devirtualized);
        devirtualized = true;
        if (imported) {
            return;
        }
        pooledBuffer = BufferPool::Get(device).Allocate(desc);
    }

    void RGBuffer::UndoDevirtualize()
    {
        Assert(devirtualized);
        devirtualized = false;
        if (imported) {
            return;
        }
        pooledBuffer = nullptr;
    }

    RGTexture::RGTexture(RGTextureDesc inDesc)
        : RGResource(RGResType::texture)
        , desc(std::move(inDesc))
        , rhiHandle(nullptr)
        , pooledTexture()
        , currentState(desc.initialState)
    {
    }

    RGTexture::RGTexture(RHI::Texture* inImportedTexture)
        : RGResource(RGResType::texture)
        , desc(RGTextureDesc::Create(inImportedTexture->GetCreateInfo()))
        , rhiHandle(inImportedTexture)
        , pooledTexture()
        , currentState(desc.initialState)
    {
        imported = true;
    }

    RGTexture::~RGTexture() = default;

    void RGTexture::Transition(RHI::CommandCommandRecorder& commandRecorder, RHI::TextureState transitionTo)
    {
        commandRecorder.ResourceBarrier(RHI::Barrier::Transition(GetRHI(), currentState, transitionTo));
        currentState = transitionTo;
    }

    const RGTextureDesc& RGTexture::GetDesc() const
    {
        return desc;
    }

    RHI::Texture* RGTexture::GetRHI() const
    {
        Assert(devirtualized);
        return rhiHandle;
    }

    void RGTexture::Devirtualize(RHI::Device& device)
    {
        Assert(!devirtualized);
        devirtualized = true;
        if (imported) {
            return;
        }
        pooledTexture = TexturePool::Get(device).Allocate(desc);
    }

    void RGTexture::UndoDevirtualize()
    {
        Assert(devirtualized);
        devirtualized = false;
        if (imported) {
            return;
        }
        pooledTexture = nullptr;
    }

    RGBufferViewDesc RGBufferViewDesc::CreateForUniform()
    {
        RGBufferViewDesc result {};
        result.type = RHI::BufferViewType::uniformBinding;
        return result;
    }

    RGBufferViewDesc RGBufferViewDesc::CreateForStorage()
    {
        RGBufferViewDesc result {};
        result.type = RHI::BufferViewType::storageBinding;
        return result;
    }

    RGBufferViewDesc RGBufferViewDesc::Create(const RHI::BufferViewCreateInfo& rhiDesc)
    {
        RGBufferViewDesc result {};
        result.type = rhiDesc.type;
        result.offset = rhiDesc.offset;
        result.size = rhiDesc.size;
        result.extend = rhiDesc.extend;
        return result;
    }

    RGTextureViewDesc RGTextureViewDesc::CreateForTexture()
    {
        RGTextureViewDesc result {};
        result.type = RHI::TextureViewType::textureBinding;
        return result;
    }

    RGTextureViewDesc RGTextureViewDesc::CreateForStorageTexture()
    {
        RGTextureViewDesc result {};
        result.type = RHI::TextureViewType::storageBinding;
        return result;
    }

    RGTextureViewDesc RGTextureViewDesc::CreateForColorAttachment()
    {
        RGTextureViewDesc result {};
        result.type = RHI::TextureViewType::colorAttachment;
        return result;
    }

    RGTextureViewDesc RGTextureViewDesc::CreateForDepthStencilAttachment()
    {
        RGTextureViewDesc result {};
        result.type = RHI::TextureViewType::depthStencil;
        return result;
    }

    RGTextureViewDesc RGTextureViewDesc::Create(const RHI::TextureViewCreateInfo& rhiDesc)
    {
        RGTextureViewDesc result {};
        result.type = rhiDesc.type;
        result.dimension = rhiDesc.dimension;
        result.aspect = rhiDesc.aspect;
        result.baseMipLevel = rhiDesc.baseMipLevel;
        result.mipLevelNum = rhiDesc.mipLevelNum;
        result.baseArrayLayer = rhiDesc.baseArrayLayer;
        result.arrayLayerNum = rhiDesc.arrayLayerNum;
        return result;
    }

    RGTextureViewDesc& RGTextureViewDesc::Dimension(RHI::TextureViewDimension inDimension)
    {
        dimension = inDimension;
        return *this;
    }

    RGTextureViewDesc& RGTextureViewDesc::Aspect(RHI::TextureAspect inAspect)
    {
        aspect = inAspect;
        return *this;
    }

    RGTextureViewDesc& RGTextureViewDesc::BaseMipLevel(uint8_t inBaseMipLevel)
    {
        baseMipLevel = inBaseMipLevel;
        return *this;
    }

    RGTextureViewDesc& RGTextureViewDesc::MipLevelNum(uint8_t inMipLevelNum)
    {
        mipLevelNum = inMipLevelNum;
        return *this;
    }

    RGTextureViewDesc& RGTextureViewDesc::BaseArrayLayer(uint8_t inBaseArrayLayer)
    {
        baseArrayLayer = inBaseArrayLayer;
        return *this;
    }

    RGTextureViewDesc& RGTextureViewDesc::ArrayLayerNum(uint8_t inArrayLayerNum)
    {
        arrayLayerNum = inArrayLayerNum;
        return *this;
    }

    RGResourceView::RGResourceView(RGResViewType inType)
        : type(inType)
        , devirtualized(false)
    {
    }

    RGResourceView::~RGResourceView() = default;

    RGResViewType RGResourceView::Type() const
    {
        return type;
    }

    RGBufferView::RGBufferView(RGBufferRef inBuffer, const RGBufferViewDesc& inDesc)
        : RGResourceView(RGResViewType::bufferView)
        , buffer(inBuffer)
        , desc(inDesc)
        , rhiHandle(nullptr)
    {
    }

    RGBufferView::~RGBufferView() = default;

    const RGBufferViewDesc& RGBufferView::GetDesc() const
    {
        return desc;
    }

    RGBufferRef RGBufferView::GetBuffer() const
    {
        return buffer;
    }

    RHI::BufferView* RGBufferView::GetRHI() const
    {
        return rhiHandle;
    }

    RGResourceRef RGBufferView::GetResource()
    {
        return buffer;
    }

    RGTextureView::RGTextureView(RGTextureRef inTexture, const RGTextureViewDesc& inDesc)
        : RGResourceView(RGResViewType::textureView)
        , desc(RGTextureViewDesc::Create(inDesc))
        , rhiHandle(nullptr)
    {
    }

    RGTextureView::~RGTextureView() = default;

    const RGTextureViewDesc& RGTextureView::GetDesc() const
    {
        return desc;
    }

    RGTextureRef RGTextureView::GetTexture() const
    {
        return texture;
    }

    RHI::TextureView* RGTextureView::GetRHI() const
    {
        return rhiHandle;
    }

    RGResourceRef RGTextureView::GetResource()
    {
        return texture;
    }

    RGBindGroupDesc RGBindGroupDesc::Create(Rendering::BindGroupLayout* inLayout)
    {
        RGBindGroupDesc result;
        result.layout = inLayout;
        return result;
    }

    RGBindGroupDesc& RGBindGroupDesc::Sampler(std::string inName, RHI::Sampler* inSampler)
    {
        Assert(!items.contains(inName));

        RGBindItemDesc item;
        item.type = RHI::BindingType::sampler;
        item.sampler = inSampler;
        items.emplace(std::make_pair(std::move(inName), std::move(item)));
        return *this;
    }

    RGBindGroupDesc& RGBindGroupDesc::UniformBuffer(std::string inName, RGBufferViewRef bufferView)
    {
        Assert(!items.contains(inName));

        RGBindItemDesc item;
        item.type = RHI::BindingType::uniformBuffer;
        item.bufferView = bufferView;
        items.emplace(std::make_pair(std::move(inName), std::move(item)));
        return *this;
    }

    RGBindGroupDesc& RGBindGroupDesc::StorageBuffer(std::string inName, RGBufferViewRef bufferView)
    {
        Assert(!items.contains(inName));

        RGBindItemDesc item;
        item.type = RHI::BindingType::storageBuffer;
        item.bufferView = bufferView;
        items.emplace(std::make_pair(std::move(inName), std::move(item)));
        return *this;
    }

    RGBindGroupDesc& RGBindGroupDesc::Texture(std::string inName, RGTextureViewRef textureView)
    {
        Assert(!items.contains(inName));

        RGBindItemDesc item;
        item.type = RHI::BindingType::texture;
        item.textureView = textureView;
        items.emplace(std::make_pair(std::move(inName), std::move(item)));
        return *this;
    }

    RGBindGroupDesc& RGBindGroupDesc::StorageTexture(std::string inName, RGTextureViewRef textureView)
    {
        Assert(!items.contains(inName));

        RGBindItemDesc item;
        item.type = RHI::BindingType::storageTexture;
        item.textureView = textureView;
        items.emplace(std::make_pair(std::move(inName), std::move(item)));
        return *this;
    }

    RGBindGroup::RGBindGroup(Rendering::RGBindGroupDesc inDesc)
        : devirtualized(false)
        , desc(std::move(inDesc))
        , rhiHandle(nullptr)
    {
    }

    RGBindGroup::~RGBindGroup() = default;

    void RGBindGroup::Devirtualize(RHI::Device& inDevice)
    {
        Assert(!devirtualized);
        if (desc.layout == nullptr) {
            devirtualized = true;
            return;
        }

        RHI::BindGroupCreateInfo createInfo(desc.layout->GetRHI());
        createInfo.entries.reserve(desc.items.size());

        for (const auto& item : desc.items) {
            const auto* binding = desc.layout->GetBinding(item.first);
            const RGBindItemDesc& itemDesc = item.second;

            if (itemDesc.type == RHI::BindingType::uniformBuffer || itemDesc.type == RHI::BindingType::storageBuffer) {
                createInfo.AddEntry(RHI::BindGroupEntry(*binding, itemDesc.bufferView->GetRHI()));
            } else if (itemDesc.type == RHI::BindingType::texture || itemDesc.type == RHI::BindingType::storageTexture) {
                createInfo.AddEntry(RHI::BindGroupEntry(*binding, itemDesc.textureView->GetRHI()));
            } else if (itemDesc.type == RHI::BindingType::sampler) {
                createInfo.AddEntry(RHI::BindGroupEntry(*binding, itemDesc.sampler));
            } else {
                Unimplement();
            }
        }

        devirtualized = true;
        rhiHandle = inDevice.CreateBindGroup(createInfo);
    }

    void RGBindGroup::UndoDevirtualize()
    {
        Assert(devirtualized);
        if (rhiHandle == nullptr) {
            return;
        }
        rhiHandle->Destroy();
    }

    const RGBindGroupDesc& RGBindGroup::GetDesc() const
    {
        return desc;
    }

    RHI::BindGroup* RGBindGroup::GetRHI() const
    {
        return rhiHandle;
    }

    RGPass::RGPass(std::string inName, RGPassType inType)
        : name(std::move(inName))
        , type(inType)
        , reads()
        , transitionInfos()
    {
    }

    RGPass::~RGPass() = default;

    void RGPass::SaveBufferTransitionInfo(RGBufferRef buffer, RHI::BufferState state)
    {
        Assert(buffer != nullptr);

        if (Internal::IsBufferStateRead(state)) {
            reads.emplace(buffer);
        }

        auto iter = transitionInfos.buffer.find(buffer);
        if (iter == transitionInfos.buffer.end()) {
            transitionInfos.buffer.emplace(std::make_pair(buffer, state));
        } else {
            if (Internal::IsBufferStateWrite(state)) {
                QuickFail();
            } else if (Internal::IsBufferStateRead(state)) {
                Assert(Internal::IsBufferStateWrite(iter->second));
            } else {
                QuickFail();
            }
        }
    }

    void RGPass::SaveTextureTransitionInfo(RGTextureRef texture, RHI::TextureState state)
    {
        Assert(texture != nullptr);

        if (Internal::IsTextureStateRead(state)) {
            reads.emplace(texture);
        }

        auto iter = transitionInfos.texture.find(texture);
        if (iter == transitionInfos.texture.end()) {
            transitionInfos.texture.emplace(std::make_pair(texture, state));
        } else {
            if (Internal::IsTextureStateWrite(state)) {
                QuickFail();
            } else if (Internal::IsTextureStateRead(state)) {
                Assert(Internal::IsTextureStateWrite(iter->second));
            } else {
                QuickFail();
            }
        }
    }

    void RGPass::CompileForBindGroups(const std::vector<RGBindGroupRef>& bindGroups)
    {
        for (const auto* bindGroup : bindGroups) {
            const auto& bindGroupDesc = bindGroup->GetDesc();
            const auto& bindItems = bindGroupDesc.items;

            // writes first
            for (const auto& bindItem : bindItems) {
                const auto& itemDesc = bindItem.second;
                if (itemDesc.type == RHI::BindingType::storageBuffer) {
                    SaveBufferTransitionInfo(itemDesc.bufferView->GetBuffer(), RHI::BufferState::storage);
                } else if (itemDesc.type == RHI::BindingType::storageTexture) {
                    SaveTextureTransitionInfo(itemDesc.textureView->GetTexture(), RHI::TextureState::storage);
                }
            }

            // reads
            for (const auto& bindItem : bindItems) {
                const auto& itemDesc = bindItem.second;
                if (itemDesc.type == RHI::BindingType::uniformBuffer) {
                    SaveBufferTransitionInfo(itemDesc.bufferView->GetBuffer(), RHI::BufferState::shaderReadOnly);
                } else if (itemDesc.type == RHI::BindingType::texture) {
                    SaveTextureTransitionInfo(itemDesc.textureView->GetTexture(), RHI::TextureState::shaderReadOnly);
                }
            }
        }
    }

    void RGPass::DevirtualizeResources(RHI::Device& device)
    {
        for (auto& read : reads) {
            read->IncRefCountAndUpdateResource(device);
        }
    }

    void RGPass::TransitionResources(RHI::CommandCommandRecorder* commandRecorder)
    {
        for (const auto& bufferTransition : transitionInfos.buffer) {
            bufferTransition.first->Transition(*commandRecorder, bufferTransition.second);
        }
        for (const auto& textureTransition : transitionInfos.texture) {
            textureTransition.first->Transition(*commandRecorder, textureTransition.second);
        }
    }

    void RGPass::FinalizeResources()
    {
        for (auto& read : reads) {
            read->DecRefAndUpdateResource();
        }
    }

    RGCopyPass::RGCopyPass(std::string inName, RGCopyPassDesc inPassDesc, RGCopyPassExecuteFunc inFunc, bool inAsyncCopy)
        : RGPass(std::move(inName), RGPassType::copy)
        , passDesc(std::move(inPassDesc))
        , func(std::move(inFunc))
        , asyncCopy(inAsyncCopy)
    {
    }

    RGCopyPass::~RGCopyPass() = default;

    void RGCopyPass::CompileForCopyPassDesc()
    {
        for (auto* resource : passDesc.copyDsts) {
            if (resource->Type() == RGResType::buffer) {
                SaveBufferTransitionInfo(static_cast<RGBufferRef>(resource), RHI::BufferState::copyDst);
            } else if (resource->Type() == RGResType::texture) {
                SaveTextureTransitionInfo(static_cast<RGTextureRef>(resource), RHI::TextureState::copyDst);
            } else {
                QuickFail();
            }
        }
        for (auto* resource : passDesc.copySrcs) {
            if (resource->Type() == RGResType::buffer) {
                SaveBufferTransitionInfo(static_cast<RGBufferRef>(resource), RHI::BufferState::copySrc);
            } else if (resource->Type() == RGResType::texture) {
                SaveTextureTransitionInfo(static_cast<RGTextureRef>(resource), RHI::TextureState::copySrc);
            } else {
                QuickFail();
            }
        }
    }

    void RGCopyPass::Compile(RGAsyncInfo& outAsyncInfo)
    {
        CompileForCopyPassDesc();
        outAsyncInfo.hasAsyncCopy = outAsyncInfo.hasAsyncCopy | asyncCopy;
    }

    void RGCopyPass::Execute(RHI::Device& device, const Internal::CommandBuffersGuard::Context& cmdBuffers)
    {
        RHI::CommandBuffer* cmdBuffer = asyncCopy ? cmdBuffers.asyncCopyCmdBuffer : cmdBuffers.mainCmdBuffer;
        Common::UniqueRef<RHI::CommandRecorder> cmdEncoder = cmdBuffer->Begin();
        {
            Common::UniqueRef<RHI::CopyPassCommandRecorder> copyCmdEncoder = cmdEncoder->BeginCopyPass();
            {
                TransitionResources(copyCmdEncoder.Get());
                func(*copyCmdEncoder);
            }
            copyCmdEncoder->EndPass();
        }
        cmdEncoder->End();
    }

    RGComputePass::RGComputePass(std::string inName, std::vector<RGBindGroupRef> inBindGroups, RGComputePassExecuteFunc inFunc, bool inAsyncCompute)
        : RGPass(std::move(inName), RGPassType::compute)
        , asyncCompute(inAsyncCompute)
        , bindGroups(std::move(inBindGroups))
        , func(std::move(inFunc))
    {
    }

    RGComputePass::~RGComputePass() = default;

    void RGComputePass::Compile(RGAsyncInfo& outAsyncInfo)
    {
        CompileForBindGroups(bindGroups);
        outAsyncInfo.hasAsyncCompute = outAsyncInfo.hasAsyncCompute | asyncCompute;
    }

    void RGComputePass::Execute(RHI::Device& device, const Internal::CommandBuffersGuard::Context& cmdBuffers)
    {
        RHI::CommandBuffer* cmdBuffer = asyncCompute ? cmdBuffers.asyncComputeCmdBuffer : cmdBuffers.mainCmdBuffer;
        Common::UniqueRef<RHI::CommandRecorder> cmdEncoder = cmdBuffer->Begin();
        {
            Common::UniqueRef<RHI::ComputePassCommandRecorder> computeCmdEncoder = cmdEncoder->BeginComputePass();
            {
                TransitionResources(computeCmdEncoder.Get());
                func(*computeCmdEncoder);
            }
            computeCmdEncoder->EndPass();
        }
        cmdEncoder->End();
    }

    RGRasterPass::RGRasterPass(std::string inName, RGRasterPassDesc inPassDesc, std::vector<RGBindGroupRef> inBindGroupds, RGRasterPassExecuteFunc inFunc)
        : RGPass(std::move(inName), RGPassType::raster)
        , passDesc(std::move(inPassDesc))
        , bindGroups(std::move(inBindGroupds))
        , func(std::move(inFunc))
    {
    }

    RGRasterPass::~RGRasterPass() = default;

    void RGRasterPass::CompileForRasterPassDesc()
    {
        for (const auto& colorAttachment: passDesc.colorAttachments) {
            SaveTextureTransitionInfo(colorAttachment.view->GetTexture(), RHI::TextureState::renderTarget);
        }
        if (passDesc.depthStencilAttachment.has_value()) {
            RGDepthStencilAttachment& depthStencilAttachment = passDesc.depthStencilAttachment.value();
            SaveTextureTransitionInfo(depthStencilAttachment.view->GetTexture(), RHI::TextureState::depthStencilWrite);
        }
    }

    void RGRasterPass::Compile(RGAsyncInfo& outAsyncInfo)
    {
        CompileForRasterPassDesc();
        CompileForBindGroups(bindGroups);
    }

    void RGRasterPass::Execute(RHI::Device& device, const Internal::CommandBuffersGuard::Context& cmdBuffers)
    {
        RHI::CommandBuffer* cmdBuffer = cmdBuffers.mainCmdBuffer;
        Common::UniqueRef<RHI::CommandRecorder> cmdEncoder = cmdBuffer->Begin();
        {
            std::optional<RHI::DepthStencilAttachment> depthStencilAttachment = Internal::GetRasterPassDepthStencilAttachment(passDesc);

            RHI::RasterPassBeginInfo passBeginInfo;
            passBeginInfo.colorAttachments = Internal::GetRasterPassColorAttachments(passDesc);
            passBeginInfo.depthStencilAttachment = depthStencilAttachment;

            Common::UniqueRef<RHI::RasterPassCommandRecorder> rasterCmdEncoder = cmdEncoder->BeginRasterPass(
                passBeginInfo);
            {
                TransitionResources(rasterCmdEncoder.Get());
                func(*rasterCmdEncoder);
            }
            rasterCmdEncoder->EndPass();
        }
        cmdEncoder->End();
    }

    RGFencePack::RGFencePack() = default;

    RGFencePack::~RGFencePack() = default;

    RGFencePack::RGFencePack(RHI::Fence* inMainFence, RHI::Fence* inAsyncComputeFence, RHI::Fence* inAsyncCopyFence)
        : mainFence(inMainFence)
        , asyncComputeFence(inAsyncComputeFence)
        , asyncCopyFence(inAsyncCopyFence)
    {
    }

    RGAsyncInfo::RGAsyncInfo()
    {
        hasAsyncCopy = false;
        hasAsyncCompute = false;
    }

    RGBuilder::RGBuilder(RHI::Device& inDevice)
        : device(inDevice)
        , executed(false)
        , asyncInfo()
    {
    }

    RGBuilder::~RGBuilder() = default;

    RGBufferRef RGBuilder::CreateBuffer(const RGBufferDesc& inDesc)
    {
        RGBufferRef result = new RGBuffer(inDesc);
        resources.emplace_back(Common::UniqueRef<RGResource>(result));
        return result;
    }

    RGTextureRef RGBuilder::CreateTexture(const RGTextureDesc& inDesc)
    {
        RGTextureRef result = new RGTexture(inDesc);
        resources.emplace_back(Common::UniqueRef<RGResource>(result));
        return result;
    }

    RGBufferViewRef RGBuilder::CreateBufferView(RGBufferRef inBuffer, const RGBufferViewDesc& inDesc)
    {
        RGBufferViewRef result = new RGBufferView(inBuffer, inDesc);
        views.emplace_back(Common::UniqueRef<RGResourceView>(result));
        return result;
    }

    RGTextureViewRef RGBuilder::CreateTextureView(RGTextureRef inTexture, const RGTextureViewDesc& inDesc)
    {
        RGTextureViewRef result = new RGTextureView(inTexture, inDesc);
        views.emplace_back(Common::UniqueRef<RGResourceView>(result));
        return result;
    }

    RGBufferRef RGBuilder::ImportBuffer(RHI::Buffer* inBuffer)
    {
        RGBufferRef result = new RGBuffer(inBuffer);
        resources.emplace_back(Common::UniqueRef<RGResource>(result));
        return result;
    }

    RGTextureRef RGBuilder::ImportTexture(RHI::Texture* inTexture)
    {
        RGTextureRef result = new RGTexture(inTexture);
        resources.emplace_back(Common::UniqueRef<RGResource>(result));
        return result;
    }

    RGBindGroupRef RGBuilder::AllocateBindGroup(const RGBindGroupDesc& inDesc)
    {
        bindGroups.emplace_back(Common::UniqueRef<RGBindGroup>(new RGBindGroup(inDesc)));
        return bindGroups.back().Get();
    }

    void RGBuilder::AddCopyPass(const std::string& inName, const RGCopyPassDesc& inPassDesc, const RGCopyPassExecuteFunc& inFunc, bool inAsyncCopy)
    {
        passes.emplace_back(Common::UniqueRef<RGPass>(new RGCopyPass(inName, inPassDesc, inFunc, inAsyncCopy)));
    }

    void RGBuilder::AddComputePass(const std::string& inName, const std::vector<RGBindGroupRef>& inBindGroups, const RGComputePassExecuteFunc& inFunc, bool inAsyncCompute)
    {
        passes.emplace_back(Common::UniqueRef<RGPass>(new RGComputePass(inName, inBindGroups, inFunc, inAsyncCompute)));
    }

    void RGBuilder::AddRasterPass(const std::string& inName, const RGRasterPassDesc& inPassDesc, const std::vector<RGBindGroupRef>& inBindGroupds, const RGRasterPassExecuteFunc& inFunc)
    {
        passes.emplace_back(Common::UniqueRef<RGPass>(new RGRasterPass(inName, inPassDesc, inBindGroupds, inFunc)));
    }

    void RGBuilder::Execute(const RGFencePack& inFencePack)
    {
        Assert(!executed);
        Compile();
        ExecuteInternal(inFencePack);
        executed = true;
    }

    void RGBuilder::Compile()
    {
        for (auto& pass : passes) {
            pass->Compile(asyncInfo);
        }

        for (auto& pass : passes) {
            pass->DevirtualizeResources(device);
        }

        for (auto& bindGroup : bindGroups) {
            bindGroup->Devirtualize(device);
        }

        for (auto& resource : resources) {
            if (resource->IsForceUsed()) {
                resource->IncRefCountAndUpdateResource(device);
            }
        }
    }

    void RGBuilder::ExecuteInternal(const Rendering::RGFencePack& inFencePack)
    {
        Internal::CommandBuffersGuard(device, asyncInfo, inFencePack, [&](const Internal::CommandBuffersGuard::Context& context) -> void {
            for (auto& pass : passes) {
                pass->Execute(device, context);
                pass->FinalizeResources();
            }
        });

        for (auto& bindGroup : bindGroups) {
            bindGroup->UndoDevirtualize();
        }
    }
}
