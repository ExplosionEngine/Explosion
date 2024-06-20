//
// Created by johnk on 2023/11/28.
//

#include <ranges>

#include <Rendering/RenderGraph.h>
#include <Rendering/RenderingCache.h>
#include <Common/Container.h>

namespace Rendering::Internal {
    static void ComputeReadsWritesForBindGroup(const RGBindGroupDesc& inDesc, std::unordered_set<RGResourceRef>& outReads, std::unordered_set<RGResourceRef>& outWrites)
    {
        for (const auto& [name, item] : inDesc.items) {
            if (item.type == RHI::BindingType::uniformBuffer) {
                outReads.emplace(std::get<RGBufferViewRef>(item.view)->GetResource());
            } else if (item.type == RHI::BindingType::storageBuffer) {
                outWrites.emplace(std::get<RGBufferViewRef>(item.view)->GetResource());
            } else if (item.type == RHI::BindingType::texture) {
                outReads.emplace(std::get<RGTextureViewRef>(item.view)->GetResource());
            } else if (item.type == RHI::BindingType::storageTexture) {
                outWrites.emplace(std::get<RGTextureViewRef>(item.view)->GetResource());
            } else {
                Unimplement();
            }
        }
    }

    static std::pair<RHI::QueueType, uint8_t> GetRHIQueueTypeAndIndex(RGQueueType inType)
    {
        if (inType == RGQueueType::main) {
            return { RHI::QueueType::graphics, 0 };
        }
        if (inType == RGQueueType::asyncCopy) {
            return { RHI::QueueType::transfer, 0 };
        }
        if (inType == RGQueueType::asyncCompute) {
            return { RHI::QueueType::compute, 0 };
        }
        Unimplement();
        return {};
    }

    static RHI::RasterPassBeginInfo GetRHIRasterPassBeginInfo(RGBuilder& builder, const RGRasterPassDesc& inDesc)
    {
        RHI::RasterPassBeginInfo result;
        if (inDesc.depthStencilAttachment.has_value()) {
            const auto& dsa = inDesc.depthStencilAttachment.value();
            result.SetDepthStencilAttachment(RHI::DepthStencilAttachment(builder.GetRHI(dsa.view), dsa.depthReadOnly, dsa.depthLoadOp, dsa.depthStoreOp, dsa.depthClearValue, dsa.stencilReadOnly, dsa.stencilLoadOp, dsa.stencilStoreOp, dsa.stencilClearValue));
        }
        for (const auto& ca : inDesc.colorAttachments) {
            result.AddColorAttachment(RHI::ColorAttachment(builder.GetRHI(ca.view), ca.loadOp, ca.storeOp, ca.clearValue));
        }
        return result;
    }
}

namespace Rendering {
    RGResource::RGResource(const RGResType inType)
        : type(inType)
        , forceUsed(false)
        , imported(false)
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

    RGBuffer::RGBuffer(RGBufferDesc inDesc)
        : RGResource(RGResType::buffer)
        , desc(std::move(inDesc))
        , rhiHandleImported(nullptr)
    {
    }

    RGBuffer::RGBuffer(RHI::Buffer* inImportedBuffer, RHI::BufferState inInitialState)
        : RGResource(RGResType::buffer)
        , desc(inImportedBuffer->GetCreateInfo())
        , rhiHandleImported(inImportedBuffer)
    {
        imported = true;
        desc.initialState = inInitialState;
    }

    RGBuffer::~RGBuffer() = default;

    const RGBufferDesc& RGBuffer::GetDesc() const
    {
        return desc;
    }

    RGTexture::RGTexture(RGTextureDesc inDesc)
        : RGResource(RGResType::texture)
        , desc(std::move(inDesc))
        , rhiHandleImported(nullptr)
    {
    }

    RGTexture::RGTexture(RHI::Texture* inImportedTexture, RHI::TextureState inInitialState)
        : RGResource(RGResType::texture)
        , desc(inImportedTexture->GetCreateInfo())
        , rhiHandleImported(inImportedTexture)
    {
        imported = true;
        desc.initialState = inInitialState;
    }

    RGTexture::~RGTexture() = default;

    const RGTextureDesc& RGTexture::GetDesc() const
    {
        return desc;
    }

    RGResourceView::RGResourceView(RGResViewType inType)
        : type(inType)
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

    RGResourceRef RGBufferView::GetResource()
    {
        return buffer;
    }

    RGTextureView::RGTextureView(RGTextureRef inTexture, const RGTextureViewDesc& inDesc)
        : RGResourceView(RGResViewType::textureView)
        , texture(inTexture)
        , desc(inDesc)
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

    RGResourceRef RGTextureView::GetResource()
    {
        return texture;
    }

    RGColorAttachment::RGColorAttachment(
        RGTextureViewRef inView,
        RHI::LoadOp inLoadOp,
        RHI::StoreOp inStoreOp,
        const Common::LinearColor& inClearValue)
        : ColorAttachmentBase(inLoadOp, inStoreOp, inClearValue)
        , view(inView)
    {

    }

    RGColorAttachment& RGColorAttachment::SetView(RGTextureViewRef inView)
    {
        view = inView;
        return *this;
    }

    RGDepthStencilAttachment::RGDepthStencilAttachment(
        RGTextureViewRef inView,
        bool inDepthReadOnly,
        RHI::LoadOp inDepthLoadOp,
        RHI::StoreOp inDepthStoreOp,
        float inDepthClearValue,
        bool inStencilReadOnly,
        RHI::LoadOp inStencilLoadOp,
        RHI::StoreOp inStencilStoreOp,
        uint32_t inStencilClearValue)
        : DepthStencilAttachmentBase(
            inDepthReadOnly, inDepthLoadOp, inDepthStoreOp, inDepthClearValue,
            inStencilReadOnly, inStencilLoadOp, inStencilStoreOp, inStencilClearValue)
        , view(inView)
    {

    }

    RGDepthStencilAttachment& RGDepthStencilAttachment::SetView(RGTextureViewRef inView)
    {
        view = inView;
        return *this;
    }

    RGRasterPassDesc& RGRasterPassDesc::AddColorAttachment(const RGColorAttachment& inAttachment)
    {
        colorAttachments.emplace_back(inAttachment);
        return *this;
    }

    RGRasterPassDesc& RGRasterPassDesc::SetDepthStencilAttachment(const RGDepthStencilAttachment& inAttachment)
    {
        depthStencilAttachment = inAttachment;
        return *this;
    }

    RGBindGroupDesc RGBindGroupDesc::Create(BindGroupLayout* inLayout)
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
        item.view = inSampler;
        items.emplace(std::make_pair(std::move(inName), item));
        return *this;
    }

    RGBindGroupDesc& RGBindGroupDesc::UniformBuffer(std::string inName, RGBufferViewRef bufferView)
    {
        Assert(!items.contains(inName));

        RGBindItemDesc item;
        item.type = RHI::BindingType::uniformBuffer;
        item.view = bufferView;
        items.emplace(std::make_pair(std::move(inName), item));
        return *this;
    }

    RGBindGroupDesc& RGBindGroupDesc::StorageBuffer(std::string inName, RGBufferViewRef bufferView)
    {
        Assert(!items.contains(inName));

        RGBindItemDesc item;
        item.type = RHI::BindingType::storageBuffer;
        item.view = bufferView;
        items.emplace(std::make_pair(std::move(inName), item));
        return *this;
    }

    RGBindGroupDesc& RGBindGroupDesc::Texture(std::string inName, RGTextureViewRef textureView)
    {
        Assert(!items.contains(inName));

        RGBindItemDesc item;
        item.type = RHI::BindingType::texture;
        item.view = textureView;
        items.emplace(std::make_pair(std::move(inName), item));
        return *this;
    }

    RGBindGroupDesc& RGBindGroupDesc::StorageTexture(std::string inName, RGTextureViewRef textureView)
    {
        Assert(!items.contains(inName));

        RGBindItemDesc item;
        item.type = RHI::BindingType::storageTexture;
        item.view = textureView;
        items.emplace(std::make_pair(std::move(inName), item));
        return *this;
    }

    RGBindGroup::RGBindGroup(RGBindGroupDesc inDesc)
        : desc(std::move(inDesc))
    {
    }

    RGBindGroup::~RGBindGroup() = default;

    const RGBindGroupDesc& RGBindGroup::GetDesc() const
    {
        return desc;
    }

    RGPass::RGPass(std::string inName, RGPassType inType)
        : name(std::move(inName))
        , type(inType)
    {
    }

    RGPass::~RGPass() = default;

    RGCopyPass::RGCopyPass(std::string inName, RGCopyPassDesc inPassDesc, RGCopyPassExecuteFunc inFunc)
        : RGPass(std::move(inName), RGPassType::copy)
        , passDesc(std::move(inPassDesc))
        , func(std::move(inFunc))
    {
    }

    RGCopyPass::~RGCopyPass() = default;

    RGComputePass::RGComputePass(std::string inName, std::vector<RGBindGroupRef> inBindGroups, RGComputePassExecuteFunc inFunc)
        : RGPass(std::move(inName), RGPassType::compute)
        , func(std::move(inFunc))
        , bindGroups(std::move(inBindGroups))
    {
    }

    RGComputePass::~RGComputePass() = default;

    RGRasterPass::RGRasterPass(std::string inName, RGRasterPassDesc inPassDesc, std::vector<RGBindGroupRef> inBindGroupds, RGRasterPassExecuteFunc inFunc)
        : RGPass(std::move(inName), RGPassType::raster)
        , passDesc(std::move(inPassDesc))
        , bindGroups(std::move(inBindGroupds))
        , func(std::move(inFunc))
    {
    }

    RGRasterPass::~RGRasterPass() = default;

    RGBuilder::RGBuilder(RHI::Device& inDevice)
        : executed(false)
        , device(inDevice)
    {
    }

    RGBuilder::~RGBuilder() = default;

    RGBufferRef RGBuilder::CreateBuffer(const RGBufferDesc& inDesc)
    {
        Assert(!executed);
        auto* const result = new RGBuffer(inDesc);
        resources.emplace_back(result);
        return result;
    }

    RGTextureRef RGBuilder::CreateTexture(const RGTextureDesc& inDesc)
    {
        Assert(!executed);
        auto* const result = new RGTexture(inDesc);
        resources.emplace_back(result);
        return result;
    }

    RGBufferViewRef RGBuilder::CreateBufferView(RGBufferRef inBuffer, const RGBufferViewDesc& inDesc)
    {
        Assert(!executed);
        auto* const result = new RGBufferView(inBuffer, inDesc);
        views.emplace_back(result);
        return result;
    }

    RGTextureViewRef RGBuilder::CreateTextureView(RGTextureRef inTexture, const RGTextureViewDesc& inDesc)
    {
        Assert(!executed);
        auto* const result = new RGTextureView(inTexture, inDesc);
        views.emplace_back(result);
        return result;
    }

    RGBufferRef RGBuilder::ImportBuffer(RHI::Buffer* inBuffer, RHI::BufferState inInitialState)
    {
        Assert(!executed);
        auto* const result = new RGBuffer(inBuffer, inInitialState);
        resources.emplace_back(result);
        return result;
    }

    RGTextureRef RGBuilder::ImportTexture(RHI::Texture* inTexture, RHI::TextureState inInitialState)
    {
        Assert(!executed);
        auto* const result = new RGTexture(inTexture, inInitialState);
        resources.emplace_back(result);
        return result;
    }

    RGBindGroupRef RGBuilder::AllocateBindGroup(const RGBindGroupDesc& inDesc)
    {
        Assert(!executed);
        bindGroups.emplace_back(new RGBindGroup(inDesc));
        return bindGroups.back().Get();
    }

    void RGBuilder::AddCopyPass(const std::string& inName, const RGCopyPassDesc& inPassDesc, const RGCopyPassExecuteFunc& inFunc, bool inAsyncCopy)
    {
        Assert(!executed);
        const auto& pass = passes.emplace_back(new RGCopyPass(inName, inPassDesc, inFunc));
        recordingAsyncTimeline[inAsyncCopy ? RGQueueType::asyncCopy : RGQueueType::main].emplace_back(pass.Get());
    }

    void RGBuilder::AddComputePass(const std::string& inName, const std::vector<RGBindGroupRef>& inBindGroups, const RGComputePassExecuteFunc& inFunc, bool inAsyncCompute)
    {
        Assert(!executed);
        const auto& pass = passes.emplace_back(new RGComputePass(inName, inBindGroups, inFunc));
        recordingAsyncTimeline[inAsyncCompute ? RGQueueType::asyncCompute : RGQueueType::main].emplace_back(pass.Get());
    }

    void RGBuilder::AddRasterPass(const std::string& inName, const RGRasterPassDesc& inPassDesc, const std::vector<RGBindGroupRef>& inBindGroupds, const RGRasterPassExecuteFunc& inFunc)
    {
        Assert(!executed);
        const auto& pass = passes.emplace_back(new RGRasterPass(inName, inPassDesc, inBindGroupds, inFunc));
        recordingAsyncTimeline[RGQueueType::main].emplace_back(pass.Get());
    }

    void RGBuilder::AddSyncPoint()
    {
        Assert(!executed);
        asyncTimelines.emplace_back(recordingAsyncTimeline);
        recordingAsyncTimeline.clear();
    }

    void RGBuilder::Execute(const RGExecuteInfo& inExecuteInfo)
    {
        Assert(!executed);
        executed = true;
        Compile();
        ExecuteInternal(inExecuteInfo);
    }

    RHI::Buffer* RGBuilder::GetRHI(RGBufferRef inBuffer) const
    {
        if (inBuffer->imported) {
            return inBuffer->rhiHandleImported;
        }
        AssertWithReason(!culledResources.contains(inBuffer), "resource has been culled");
        AssertWithReason(devirtualizedResources.contains(inBuffer), "resource was not devirtualized or has been released");
        return std::get<PooledBufferRef>(devirtualizedResources.at(inBuffer))->GetRHI();
    }

    RHI::Texture* RGBuilder::GetRHI(RGTextureRef inTexture) const
    {
        if (inTexture->imported) {
            return inTexture->rhiHandleImported;
        }
        AssertWithReason(!culledResources.contains(inTexture), "resource has been culled");
        AssertWithReason(devirtualizedResources.contains(inTexture), "resource was not devirtualized or has been released");
        return std::get<PooledTextureRef>(devirtualizedResources.at(inTexture))->GetRHI();
    }

    RHI::BufferView* RGBuilder::GetRHI(RGBufferViewRef inBufferView) const
    {
        AssertWithReason(!culledResources.contains(inBufferView->GetResource()), "resource has been culled");
        AssertWithReason(devirtualizedResources.contains(inBufferView->GetResource()), "resource was not devirtualized or has been released");
        AssertWithReason(devirtualizedResourceViews.contains(inBufferView), "resource view was not devirtualized or has been released");
        return std::get<RHI::BufferView*>(devirtualizedResourceViews.at(inBufferView));
    }

    RHI::TextureView* RGBuilder::GetRHI(RGTextureViewRef inTextureView) const
    {
        AssertWithReason(!culledResources.contains(inTextureView->GetResource()), "resource has been culled");
        AssertWithReason(devirtualizedResources.contains(inTextureView->GetResource()), "resource was not devirtualized or has been released");
        AssertWithReason(devirtualizedResourceViews.contains(inTextureView), "resource view was not devirtualized or has been released");
        return std::get<RHI::TextureView*>(devirtualizedResourceViews.at(inTextureView));
    }

    RHI::BindGroup* RGBuilder::GetRHI(RGBindGroupRef inBindGroup) const
    {
        AssertWithReason(devirtualizedBindGroups.contains(inBindGroup), "bind group was not devirtualized or has been released");
        return devirtualizedBindGroups.at(inBindGroup).Get();
    }

    RGBuilder::AsyncTimelineExecuteContext::AsyncTimelineExecuteContext() = default;

    RGBuilder::AsyncTimelineExecuteContext::AsyncTimelineExecuteContext(AsyncTimelineExecuteContext&& inOther) noexcept
        : cmdBuffers(std::move(inOther.cmdBuffers))
        , semaphores(std::move(inOther.semaphores))
        , queueCmdBufferMap(std::move(inOther.queueCmdBufferMap))
        , queueSemaphoreToSignalMap(std::move(inOther.queueSemaphoreToSignalMap))
    {
    }

    void RGBuilder::Compile()
    {
        CompilePassReadWrites();
        PerformCull();
    }

    void RGBuilder::ExecuteInternal(const RGExecuteInfo& inExecuteInfo) // NOLINT
    {
        const auto asyncTimelineNum = asyncTimelines.size();
        asyncTimelineExecuteContexts.reserve(asyncTimelineNum);

        for (const auto& queuePasses : asyncTimelines) {
            const bool isFirstAsyncTimeline = asyncTimelineExecuteContexts.empty();
            const bool isLastAsyncTimeline = asyncTimelineExecuteContexts.size() + 1 == asyncTimelines.size();

            std::vector<RHI::Semaphore*> semaphoresToWait;
            if (isFirstAsyncTimeline) {
                // if is first async timeline, need wait semaphore from builder outside
                for (auto* semaphore : inExecuteInfo.semaphoresToWait) {
                    semaphoresToWait.emplace_back(semaphore);
                }
            } else {
                // wait all cmd buffers in last async timeline executed
                for (const AsyncTimelineExecuteContext& lastContext = asyncTimelineExecuteContexts.back();
                    const auto& semaphore : lastContext.semaphores) {
                    semaphoresToWait.emplace_back(semaphore.Get());
                }
            }

            auto& [commandBuffers, semaphores, commandBufferMap, semaphoreMap] = asyncTimelineExecuteContexts.emplace_back();

            const auto queueNumInAsyncTimeline = queuePasses.size();
            commandBuffers.reserve(queueNumInAsyncTimeline);
            semaphores.reserve(queueNumInAsyncTimeline);
            commandBufferMap.reserve(queueNumInAsyncTimeline);
            semaphoreMap.reserve(queueNumInAsyncTimeline);

            for (const auto& [queueType, passes] : queuePasses) {
                auto& commandBuffer = commandBuffers.emplace_back(device.CreateCommandBuffer());
                auto& semaphore = semaphores.emplace_back(isLastAsyncTimeline ? nullptr : device.CreateSemaphore());
                commandBufferMap.emplace(std::make_pair(queueType, commandBuffer.Get()));
                semaphoreMap.emplace(std::make_pair(queueType, isLastAsyncTimeline ? nullptr : semaphore.Get()));

                auto* commandBufferToRecord = commandBufferMap.at(queueType);
                auto* semaphoreToSignal = semaphoreMap.at(queueType);

                {
                    auto commandRecorder = commandBufferToRecord->Begin();
                    for (auto* pass : passes) {
                        if (pass->type == RGPassType::copy) {
                            ExecuteCopyPass(*commandRecorder, static_cast<RGCopyPass*>(pass));
                        } else if (pass->type == RGPassType::compute) {
                            ExecuteComputePass(*commandRecorder, static_cast<RGComputePass*>(pass));
                        } else if (pass->type == RGPassType::raster) {
                            ExecuteRasterPass(*commandRecorder, static_cast<RGRasterPass*>(pass));
                        } else {
                            Unimplement();
                        }
                    }
                    commandRecorder->End();
                }

                auto [rhiQueueType, rhiQueueIndex] = Internal::GetRHIQueueTypeAndIndex(queueType);
                auto submitInfo = RHI::QueueSubmitInfo()
                    .SetWaitSemaphores(semaphoresToWait);
                if (isLastAsyncTimeline) {
                    // if is last async timeline, need notify all commands inside build has been executed
                    for (auto* finalSignalSemaphore : inExecuteInfo.semaphoresToSignal) {
                        submitInfo.AddSignalSemaphore(finalSignalSemaphore);
                    }
                } else {
                    // if within the builder, just wait last async timeline commands executed
                    submitInfo.AddSignalSemaphore(semaphoreToSignal);
                }
                if (queueType == RGQueueType::main && isLastAsyncTimeline && inExecuteInfo.inFenceToSignal != nullptr) {
                    // if is last async timeline, also need signal fence to notify CPU if needed
                    submitInfo.SetSignalFence(inExecuteInfo.inFenceToSignal);
                }

                device
                    .GetQueue(rhiQueueType, rhiQueueIndex)
                    ->Submit(commandBufferToRecord, submitInfo);
            }
        }
    }

    void RGBuilder::CompilePassReadWrites() // NOLINT
    {
        for (const auto& pass : passes) {
            auto* passRef = pass.Get();
            Assert(!passReadsMap.contains(passRef));
            Assert(!passWritesMap.contains(passRef));
            passReadsMap.emplace(std::make_pair(passRef, std::unordered_set<RGResourceRef> {}));
            passWritesMap.emplace(std::make_pair(passRef, std::unordered_set<RGResourceRef> {}));
            auto& passReads = passReadsMap.at(passRef);
            auto& passWrites = passWritesMap.at(passRef);

            if (passRef->type == RGPassType::copy) {
                const auto* copyPass = static_cast<RGCopyPass*>(passRef);
                for (auto* copySrc : copyPass->passDesc.copySrcs) {
                    passReads.emplace(copySrc);
                }
                for (auto* copyDst : copyPass->passDesc.copyDsts) {
                    passWrites.emplace(copyDst);
                }
            } else if (passRef->type == RGPassType::compute) {
                for (const auto* computePass = static_cast<RGComputePass*>(passRef);
                    const auto* bindGroup : computePass->bindGroups) {
                    Internal::ComputeReadsWritesForBindGroup(bindGroup->desc, passReads, passWrites);
                }
            } else if (passRef->type == RGPassType::raster) {
                const auto* rasterPass = static_cast<RGRasterPass*>(passRef);
                for (const auto* bindGroup : rasterPass->bindGroups) {
                    Internal::ComputeReadsWritesForBindGroup(bindGroup->desc, passReads, passWrites);
                }

                const auto& [colorAttachments, depthStencilAttachment] = rasterPass->passDesc;
                if (depthStencilAttachment.has_value()) {
                    passWrites.emplace(depthStencilAttachment.value().view->GetResource());
                }
                for (const auto& colorAttachment : colorAttachments) {
                    passWrites.emplace(colorAttachment.view->GetResource());
                }
            } else {
                Unimplement();
            }
        }

        for (const auto& resource : resources) {
            resourceReadCounts[resource.Get()] = resource->forceUsed ? 1 : 0;
        }
        for (const auto& pass : passes) {
            for (auto* read : passReadsMap.at(pass.Get())) {
                resourceReadCounts[read]++;
            }
        }
    }

    void RGBuilder::PerformSyncCheck() const
    {
        auto collectQueueReadWrites = [this](const std::vector<RGPassRef>& passes, std::unordered_set<RGResourceRef>& outReads, std::unordered_set<RGResourceRef>& outWrites) -> void {
            for (auto* pass : passes) {
                Common::SetUtils::GetUnionInline(outReads, passReadsMap.at(pass));
                Common::SetUtils::GetUnionInline(outWrites, passWritesMap.at(pass));
            }
        };

        for (const auto& queuePasses : asyncTimelines) {
            std::vector<std::unordered_set<RGResourceRef>> queueReadsVec;
            std::vector<std::unordered_set<RGResourceRef>> queueWritesVec;
            queueReadsVec.reserve(queuePasses.size());
            queueWritesVec.reserve(queuePasses.size());

            for (const auto& [queueType, passes] : queuePasses) {
                auto& queueReads = queueReadsVec.emplace_back();
                auto& queueWrites = queueWritesVec.emplace_back();
                collectQueueReadWrites(passes, queueReads, queueWrites);
            }

            Assert(queueReadsVec.size() == queueWritesVec.size());
            const auto size = queueReadsVec.size();
            for (auto i = 0; i < size; i++) {
                for (auto j = 0; j < size; j++) {
                    if (i == j) {
                        continue;
                    }
                    auto intersection = Common::SetUtils::GetIntersection(queueReadsVec[i], queueWritesVec[j]);
                    AssertWithReason(intersection.empty(), "async execution do not allow read a resource which writing by another queue, use AddSyncPoint() to split them");

                    intersection = Common::SetUtils::GetIntersection(queueWritesVec[i], queueWritesVec[j]);
                    AssertWithReason(intersection.empty(), "async execution do not allow write to a resource at same time in different queues, use AddSyncPoint() to split them");
                }
            }
        }
    }

    void RGBuilder::PerformCull()
    {
        // initial cull
        for (const auto& resource : resources) {
            if (auto* resourceRef = resource.Get();
                resourceReadCounts.at(resourceRef) == 0) {
                culledResources.emplace(resourceRef);
            }
        }

        // iterative cull
        for (auto riter = passes.rbegin(); riter != passes.rend(); ++riter) {
            const auto& pass = riter->Get();
            const auto& passWrites = passWritesMap.at(pass);

            bool allWritesCulled = true;
            for (auto* write : passWrites) {
                if (!culledResources.contains(write)) {
                    allWritesCulled = false;
                    break;
                }
            }

            if (!allWritesCulled) {
                continue;
            }
            culledPasses.emplace(pass);
            for (const auto& passReads = passReadsMap.at(pass);
                auto* read : passReads) {
                if (auto& readCount = resourceReadCounts.at(read);
                    --readCount == 0) {
                    culledResources.emplace(read);
                }
            }
        }
    }

    void RGBuilder::ComputeResourcesInitialState()
    {
        for (const auto& resource : resources) {
            auto* resourceRef = resource.Get();
            if (culledResources.contains(resourceRef)) {
                continue;
            }

            if (resourceRef->type == RGResType::buffer) {
                resourceStates[resourceRef] = static_cast<RGBufferRef>(resourceRef)->desc.initialState;
            } else if (resourceRef->type == RGResType::texture) {
                resourceStates[resourceRef] = static_cast<RGTextureRef>(resourceRef)->desc.initialState;
            } else {
                Unimplement();
            }
        }
    }

    void RGBuilder::ExecuteCopyPass(RHI::CommandRecorder& inRecoder, RGCopyPass* inCopyPass)
    {
        DevirtualizeResources(passWritesMap.at(inCopyPass));
        {
            const auto copyPassRecoder = inRecoder.BeginCopyPass();
            {
                TransitionResourcesForCopyPassDesc(inRecoder, inCopyPass->passDesc);
                inCopyPass->func(*this, *copyPassRecoder);
            }
            copyPassRecoder->EndPass();
        }
        FinalizePassResources(passReadsMap.at(inCopyPass));
    }

    void RGBuilder::ExecuteComputePass(RHI::CommandRecorder& inRecoder, RGComputePass* inComputePass)
    {
        DevirtualizeResources(passWritesMap.at(inComputePass));
        {
            const auto computePassRecoder = inRecoder.BeginComputePass();
            {
                TransitionResourcesForBindGroups(inRecoder, inComputePass->bindGroups);
                inComputePass->func(*this, *computePassRecoder);
            }
            computePassRecoder->EndPass();
        }
        FinalizePassResources(passReadsMap.at(inComputePass));
        FinalizePassBindGroups(inComputePass->bindGroups);
    }

    void RGBuilder::ExecuteRasterPass(RHI::CommandRecorder& inRecoder, RGRasterPass* inRasterPass)
    {
        DevirtualizeResources(passWritesMap.at(inRasterPass));
        {
            const auto rasterPassRecoder = inRecoder.BeginRasterPass(Internal::GetRHIRasterPassBeginInfo(*this, inRasterPass->passDesc));
            {
                TransitionResourcesForBindGroups(inRecoder, inRasterPass->bindGroups);
                TransitionResourcesForRasterPassDesc(inRecoder, inRasterPass->passDesc);
                inRasterPass->func(*this, *rasterPassRecoder);
            }
            rasterPassRecoder->EndPass();
        }
        FinalizePassResources(passReadsMap.at(inRasterPass));
        FinalizePassBindGroups(inRasterPass->bindGroups);
    }

    void RGBuilder::DevirtualizeResources(const std::unordered_set<RGResourceRef>& inResources)
    {
        for (auto* resource : inResources) {
            if (culledResources.contains(resource)
                || devirtualizedResources.contains(resource)) {
                continue;
            }

            if (resource->type == RGResType::buffer) {
                devirtualizedResources.emplace(std::make_pair(resource, BufferPool::Get(device).Allocate(static_cast<RGBufferRef>(resource)->desc)));
            } else if (resource->type == RGResType::texture) {
                devirtualizedResources.emplace(std::make_pair(resource, TexturePool::Get(device).Allocate(static_cast<RGTextureRef>(resource)->desc)));
            } else {
                Unimplement();
            }
        }
    }

    void RGBuilder::DevirtualizeBindGroupsAndViews(const std::vector<RGBindGroupRef>& inBindGroups)
    {
        for (auto* bindGroup : inBindGroups) {
            const auto& [layout, items] = bindGroup->desc;
            RHI::BindGroupCreateInfo createInfo(layout->GetRHI());

            for (const auto& [name, item] : items) {
                const auto* binding = layout->GetBinding(name);
                Assert(binding != nullptr);

                if (item.type == RHI::BindingType::uniformBuffer || item.type == RHI::BindingType::storageBuffer) {
                    auto* bufferView = std::get<RGBufferViewRef>(item.view);
                    if (!devirtualizedResourceViews.contains(bufferView)) {
                        devirtualizedResourceViews.emplace(std::make_pair(bufferView, ResourceViewCache::Get(device).GetOrCreate(GetRHI(bufferView->GetBuffer()), bufferView->desc)));
                    }
                    createInfo.AddEntry(RHI::BindGroupEntry(*binding, GetRHI(bufferView)));
                } else if (item.type == RHI::BindingType::texture || item.type == RHI::BindingType::storageTexture) {
                    auto* textureView = std::get<RGTextureViewRef>(item.view);
                    if (!devirtualizedResourceViews.contains(textureView)) {
                        devirtualizedResourceViews.emplace(std::make_pair(textureView, ResourceViewCache::Get(device).GetOrCreate(GetRHI(textureView->GetTexture()), textureView->desc)));
                    }
                    createInfo.AddEntry(RHI::BindGroupEntry(*binding, GetRHI(textureView)));
                } else if (item.type == RHI::BindingType::sampler) {
                    createInfo.AddEntry(RHI::BindGroupEntry(*binding, std::get<RHI::Sampler*>(item.view)));
                } else {
                    Unimplement();
                }
            }
            devirtualizedBindGroups.emplace(std::make_pair(bindGroup, device.CreateBindGroup(createInfo)));
        }
    }

    void RGBuilder::FinalizePassResources(const std::unordered_set<RGResourceRef>& inResources)
    {
        for (auto* resource : inResources) {
            if (auto& readCount = resourceReadCounts.at(resource);
                --readCount == 0) {
                if (resource->type == RGResType::buffer) {
                    ResourceViewCache::Get(device).Invalidate(std::get<PooledBufferRef>(devirtualizedResources.at(resource))->GetRHI());
                } else if (resource->type == RGResType::texture) {
                    ResourceViewCache::Get(device).Invalidate(std::get<PooledTextureRef>(devirtualizedResources.at(resource))->GetRHI());
                } else {
                    Unimplement();
                }
                devirtualizedResources.erase(resource);
            }
        }
    }

    void RGBuilder::FinalizePassBindGroups(const std::vector<RGBindGroupRef>& inBindGroups)
    {
        for (auto* bindGroup : inBindGroups) {
            devirtualizedBindGroups.erase(bindGroup);
        }
    }

    void RGBuilder::TransitionResourcesForCopyPassDesc(RHI::CommandCommandRecorder& inRecoder, const RGCopyPassDesc& inDesc)
    {
        for (auto* copySrc : inDesc.copySrcs) {
            if (copySrc->type == RGResType::buffer) {
                TransitionBuffer(inRecoder, static_cast<RGBufferRef>(copySrc), RHI::BufferState::copySrc);
            } else if (copySrc->type == RGResType::texture) {
                TransitionTexture(inRecoder, static_cast<RGTextureRef>(copySrc), RHI::TextureState::copySrc);
            } else {
                Unimplement();
            }
        }
        for (auto* copyDst : inDesc.copyDsts) {
            if (copyDst->type == RGResType::buffer) {
                TransitionBuffer(inRecoder, static_cast<RGBufferRef>(copyDst), RHI::BufferState::copyDst);
            } else if (copyDst->type == RGResType::texture) {
                TransitionTexture(inRecoder, static_cast<RGTextureRef>(copyDst), RHI::TextureState::copyDst);
            } else {
                Unimplement();
            }
        }
    }

    void RGBuilder::TransitionResourcesForRasterPassDesc(RHI::CommandCommandRecorder& inRecoder, const RGRasterPassDesc& inDesc)
    {
        if (inDesc.depthStencilAttachment.has_value()) {
            const auto& dsa = inDesc.depthStencilAttachment.value();
            TransitionTexture(inRecoder, dsa.view->GetTexture(), dsa.depthReadOnly ? RHI::TextureState::depthStencilReadonly : RHI::TextureState::depthStencilWrite);
        }
        for (const auto& ca : inDesc.colorAttachments) {
            TransitionTexture(inRecoder, ca.view->GetTexture(), RHI::TextureState::renderTarget);
        }
    }

    void RGBuilder::TransitionResourcesForBindGroups(RHI::CommandCommandRecorder& inRecoder, const std::vector<RGBindGroupRef>& inBindGroups)
    {
        for (auto* bindGroup : inBindGroups) {
            for (const auto& [name, item] : bindGroup->desc.items) {
                if (item.type == RHI::BindingType::uniformBuffer) {
                    TransitionBuffer(inRecoder, std::get<RGBufferViewRef>(item.view)->GetBuffer(), RHI::BufferState::shaderReadOnly);
                } else if (item.type == RHI::BindingType::storageBuffer) {
                    TransitionBuffer(inRecoder, std::get<RGBufferViewRef>(item.view)->GetBuffer(), RHI::BufferState::storage);
                } else if (item.type == RHI::BindingType::texture) {
                    TransitionTexture(inRecoder, std::get<RGTextureViewRef>(item.view)->GetTexture(), RHI::TextureState::shaderReadOnly);
                } else if (item.type == RHI::BindingType::storageTexture) {
                    TransitionTexture(inRecoder, std::get<RGTextureViewRef>(item.view)->GetTexture(), RHI::TextureState::storage);
                } else if (item.type == RHI::BindingType::sampler) {} else {
                    Unimplement();
                }
            }
        }
    }

    void RGBuilder::TransitionBuffer(RHI::CommandCommandRecorder& inRecoder, RGBufferRef inBuffer, RHI::BufferState inState)
    {
        auto& currentState = std::get<RHI::BufferState>(resourceStates.at(inBuffer));
        if (currentState == inState) {
            return;
        }
        inRecoder.ResourceBarrier(RHI::Barrier::Transition(GetRHI(inBuffer), currentState, inState));
        currentState = inState;
    }

    void RGBuilder::TransitionTexture(RHI::CommandCommandRecorder& inRecoder, RGTextureRef inTexture, RHI::TextureState inState)
    {
        auto& currentState = std::get<RHI::TextureState>(resourceStates.at(inTexture));
        if (currentState == inState) {
            return;
        }
        inRecoder.ResourceBarrier(RHI::Barrier::Transition(GetRHI(inTexture), currentState, inState));
        currentState = inState;
    }
}
