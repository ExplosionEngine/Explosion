//
// Created by johnk on 2023/11/28.
//

#include <ranges>

#include <Render/RenderGraph.h>
#include <Render/RenderThread.h>
#include <Common/Container.h>

namespace Render::Internal {
    static void ComputeReadsWritesForBindGroup(const RGBindGroupDesc& inDesc, std::unordered_set<RGResourceRef>& outReads, std::unordered_set<RGResourceRef>& outWrites)
    {
        for (const auto& [type, view] : inDesc.items | std::views::values) {
            if (type == RHI::BindingType::uniformBuffer) {
                outReads.emplace(std::get<RGBufferViewRef>(view)->GetResource());
            } else if (type == RHI::BindingType::storageBuffer) {
                outReads.emplace(std::get<RGBufferViewRef>(view)->GetResource());
            } else if (type == RHI::BindingType::rwStorageBuffer) {
                outWrites.emplace(std::get<RGBufferViewRef>(view)->GetResource());
            } else if (type == RHI::BindingType::texture) {
                outReads.emplace(std::get<RGTextureViewRef>(view)->GetResource());
            } else if (type == RHI::BindingType::storageTexture) {
                outWrites.emplace(std::get<RGTextureViewRef>(view)->GetResource());
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

namespace Render {
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

    RGBindGroupDesc& RGBindGroupDesc::RwStorageBuffer(std::string inName, RGBufferViewRef bufferView)
    {
        Assert(!items.contains(inName));

        RGBindItemDesc item;
        item.type = RHI::BindingType::rwStorageBuffer;
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

    RGBufferUploadInfo::RGBufferUploadInfo()
        : data(nullptr)
        , size(0)
        , srcOffset(0)
        , dstOffset(0)
    {
    }

    RGBufferUploadInfo::RGBufferUploadInfo(void* inData, size_t inSize, size_t inSrcOffset, size_t inDstOffset)
        : data(inData)
        , size(inSize)
        , srcOffset(inSrcOffset)
        , dstOffset(inDstOffset)
    {
    }

    RGPass::RGPass(std::string inName, RGPassType inType)
        : name(std::move(inName))
        , type(inType)
    {
    }

    RGPass::~RGPass() = default;

    RGCopyPass::RGCopyPass(std::string inName, RGCopyPassDesc inPassDesc, RGCopyPassExecuteFunc inFunc, RGCommonPassExecuteFunc inPreExecuteFunc, RGCommonPassExecuteFunc inPostExecuteFunc)
        : RGPass(std::move(inName), RGPassType::copy)
        , passDesc(std::move(inPassDesc))
        , passFunc(std::move(inFunc))
        , prePassFunc(std::move(inPreExecuteFunc))
        , postPassFunc(std::move(inPostExecuteFunc))
    {
    }

    RGCopyPass::~RGCopyPass() = default;

    RGComputePass::RGComputePass(std::string inName, std::vector<RGBindGroupRef> inBindGroups, RGComputePassExecuteFunc inFunc, RGCommonPassExecuteFunc inPreExecuteFunc, RGCommonPassExecuteFunc inPostExecuteFunc)
        : RGPass(std::move(inName), RGPassType::compute)
        , passFunc(std::move(inFunc))
        , prePassFunc(std::move(inPreExecuteFunc))
        , postPassFunc(std::move(inPostExecuteFunc))
        , bindGroups(std::move(inBindGroups))
    {
    }

    RGComputePass::~RGComputePass() = default;

    RGRasterPass::RGRasterPass(std::string inName, RGRasterPassDesc inPassDesc, std::vector<RGBindGroupRef> inBindGroups, RGRasterPassExecuteFunc inFunc, RGCommonPassExecuteFunc inPreExecuteFunc, RGCommonPassExecuteFunc inPostExecuteFunc)
        : RGPass(std::move(inName), RGPassType::raster)
        , passDesc(std::move(inPassDesc))
        , passFunc(std::move(inFunc))
        , prePassFunc(std::move(inPreExecuteFunc))
        , postPassFunc(std::move(inPostExecuteFunc))
        , bindGroups(std::move(inBindGroups))
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

    void RGBuilder::QueueBufferUpload(RGBufferRef inBuffer, const RGBufferUploadInfo& inUploadInfo)
    {
        Assert((inBuffer->GetDesc().usages & RHI::BufferUsageBits::mapWrite) != RHI::BufferUsageFlags::null);
        bufferUploads.emplace(inBuffer, inUploadInfo);
    }

    void RGBuilder::AddCopyPass(const std::string& inName, const RGCopyPassDesc& inPassDesc, const RGCopyPassExecuteFunc& inFunc, bool inAsyncCopy, const RGCommonPassExecuteFunc& inPreExecuteFunc, const RGCommonPassExecuteFunc& inPostExecuteFunc)
    {
        Assert(!executed);
        const auto& pass = passes.emplace_back(new RGCopyPass(inName, inPassDesc, inFunc, inPreExecuteFunc, inPostExecuteFunc));
        recordingAsyncTimeline[inAsyncCopy ? RGQueueType::asyncCopy : RGQueueType::main].emplace_back(pass.Get());
    }

    void RGBuilder::AddComputePass(const std::string& inName, const std::vector<RGBindGroupRef>& inBindGroups, const RGComputePassExecuteFunc& inFunc, bool inAsyncCompute, const RGCommonPassExecuteFunc& inPreExecuteFunc, const RGCommonPassExecuteFunc& inPostExecuteFunc)
    {
        Assert(!executed);
        const auto& pass = passes.emplace_back(new RGComputePass(inName, inBindGroups, inFunc, inPreExecuteFunc, inPostExecuteFunc));
        recordingAsyncTimeline[inAsyncCompute ? RGQueueType::asyncCompute : RGQueueType::main].emplace_back(pass.Get());
    }

    void RGBuilder::AddRasterPass(const std::string& inName, const RGRasterPassDesc& inPassDesc, const std::vector<RGBindGroupRef>& inBindGroups, const RGRasterPassExecuteFunc& inFunc, const RGCommonPassExecuteFunc& inPreExecuteFunc, const RGCommonPassExecuteFunc& inPostExecuteFunc)
    {
        Assert(!executed);
        const auto& pass = passes.emplace_back(new RGRasterPass(inName, inPassDesc, inBindGroups, inFunc, inPreExecuteFunc, inPostExecuteFunc));
        recordingAsyncTimeline[RGQueueType::main].emplace_back(pass.Get());
    }

    void RGBuilder::AddSyncPoint()
    {
        Assert(!executed);
        if (recordingAsyncTimeline.empty()) {
            return;
        }
        asyncTimelines.emplace_back(recordingAsyncTimeline);
        recordingAsyncTimeline.clear();
    }

    void RGBuilder::Execute(const RGExecuteInfo& inExecuteInfo)
    {
        Assert(!executed);
        AddSyncPoint();
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
        auto* resource = inBufferView->GetResource();
        AssertWithReason(!culledResources.contains(resource), "resource has been culled");
        AssertWithReason(resource->imported || devirtualizedResources.contains(resource), "resource was not devirtualized or has been released");
        AssertWithReason(devirtualizedResourceViews.contains(inBufferView), "resource view was not devirtualized or has been released");
        return std::get<RHI::BufferView*>(devirtualizedResourceViews.at(inBufferView));
    }

    RHI::TextureView* RGBuilder::GetRHI(RGTextureViewRef inTextureView) const
    {
        auto* resource = inTextureView->GetResource();
        AssertWithReason(!culledResources.contains(resource), "resource has been culled");
        AssertWithReason(resource->imported || devirtualizedResources.contains(resource), "resource was not devirtualized or has been released");
        AssertWithReason(devirtualizedResourceViews.contains(inTextureView), "resource view was not devirtualized or has been released");
        return std::get<RHI::TextureView*>(devirtualizedResourceViews.at(inTextureView));
    }

    RHI::BindGroup* RGBuilder::GetRHI(RGBindGroupRef inBindGroup) const
    {
        AssertWithReason(devirtualizedBindGroups.contains(inBindGroup), "bind group was not devirtualized or has been released");
        return devirtualizedBindGroups.at(inBindGroup);
    }

    RGBuilder::AsyncTimelineExecuteContext::AsyncTimelineExecuteContext() = default;

    RGBuilder::AsyncTimelineExecuteContext::AsyncTimelineExecuteContext(AsyncTimelineExecuteContext&& inOther) noexcept // NOLINT
        : queueCmdBufferMap(std::move(inOther.queueCmdBufferMap))
        , queueSemaphoreToSignalMap(std::move(inOther.queueSemaphoreToSignalMap))
    {
    }

    void RGBuilder::Compile()
    {
        CompilePassReadWrites();
        PerformCull();
        ComputeResourcesInitialState();
    }

    void RGBuilder::ExecuteInternal(const RGExecuteInfo& inExecuteInfo) // NOLINT
    {
        PerformBufferUploads();
        DevirtualizeViewsCreatedOnImportedResources();

        const auto asyncTimelineNum = asyncTimelines.size();
        asyncTimelineExecuteContexts.reserve(asyncTimelineNum);

        WaitBufferUploadsFinish();
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
                    const auto& semaphore : lastContext.queueSemaphoreToSignalMap | std::views::values) {
                    semaphoresToWait.emplace_back(semaphore.Get());
                }
            }

            auto& [commandBufferMap, semaphoreMap] = asyncTimelineExecuteContexts.emplace_back();

            const auto queueNumInAsyncTimeline = queuePasses.size();
            commandBufferMap.reserve(queueNumInAsyncTimeline);
            semaphoreMap.reserve(queueNumInAsyncTimeline);

            for (const auto& [queueType, passes] : queuePasses) {
                commandBufferMap.emplace(queueType, device.CreateCommandBuffer());
                semaphoreMap.emplace(queueType, isLastAsyncTimeline ? nullptr : device.CreateSemaphore());

                auto& commandBufferToRecord = commandBufferMap.at(queueType);
                auto& semaphoreToSignal = semaphoreMap.at(queueType);

                {
                    auto commandRecorder = commandBufferToRecord->Begin();
                    for (auto* pass : passes) {
                        if (culledPasses.contains(pass)) {
                            continue;
                        }

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
                    submitInfo.AddSignalSemaphore(semaphoreToSignal.Get());
                }
                if (queueType == RGQueueType::main && isLastAsyncTimeline && inExecuteInfo.inFenceToSignal != nullptr) {
                    // if is last async timeline, also need signal fence to notify CPU if needed
                    submitInfo.SetSignalFence(inExecuteInfo.inFenceToSignal);
                }

                device
                    .GetQueue(rhiQueueType, rhiQueueIndex)
                    ->Submit(commandBufferToRecord.Get(), submitInfo);
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
            resourceReadCounts[resource.Get()] = resource->forceUsed || resource->imported ? 1 : 0;
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
                Common::SetUtils::GetUnionInplace(outReads, passReadsMap.at(pass));
                Common::SetUtils::GetUnionInplace(outWrites, passWritesMap.at(pass));
            }
        };

        for (const auto& queuePasses : asyncTimelines) {
            std::vector<std::unordered_set<RGResourceRef>> queueReadsVec;
            std::vector<std::unordered_set<RGResourceRef>> queueWritesVec;
            queueReadsVec.reserve(queuePasses.size());
            queueWritesVec.reserve(queuePasses.size());

            for (const auto& passes : queuePasses | std::views::values) {
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
            TransitionResourcesForCopyPassDesc(inRecoder, inCopyPass->passDesc);
            if (inCopyPass->prePassFunc) {
                inCopyPass->prePassFunc(*this, inRecoder);
            }
            {
                const auto copyPassRecoder = inRecoder.BeginCopyPass();
                inCopyPass->passFunc(*this, *copyPassRecoder);
                copyPassRecoder->EndPass();
            }
            if (inCopyPass->postPassFunc) {
                inCopyPass->postPassFunc(*this, inRecoder);
            }
        }
        FinalizePassResources(passReadsMap.at(inCopyPass));
    }

    void RGBuilder::ExecuteComputePass(RHI::CommandRecorder& inRecoder, RGComputePass* inComputePass)
    {
        DevirtualizeResources(passWritesMap.at(inComputePass));
        DevirtualizeBindGroupsAndViews(inComputePass->bindGroups);
        {
            TransitionResourcesForBindGroups(inRecoder, inComputePass->bindGroups);
            if (inComputePass->prePassFunc) {
                inComputePass->prePassFunc(*this, inRecoder);
            }
            {
                const auto computePassRecoder = inRecoder.BeginComputePass();
                inComputePass->passFunc(*this, *computePassRecoder);
                computePassRecoder->EndPass();
            }
            if (inComputePass->postPassFunc) {
                inComputePass->postPassFunc(*this, inRecoder);
            }
        }
        FinalizePassResources(passReadsMap.at(inComputePass));
        FinalizePassBindGroups(inComputePass->bindGroups);
    }

    void RGBuilder::ExecuteRasterPass(RHI::CommandRecorder& inRecoder, RGRasterPass* inRasterPass)
    {
        DevirtualizeResources(passWritesMap.at(inRasterPass));
        DevirtualizeAttachmentViews(inRasterPass->passDesc);
        DevirtualizeBindGroupsAndViews(inRasterPass->bindGroups);
        {
            TransitionResourcesForBindGroups(inRecoder, inRasterPass->bindGroups);
            TransitionResourcesForRasterPassDesc(inRecoder, inRasterPass->passDesc);
            if (inRasterPass->prePassFunc) {
                inRasterPass->prePassFunc(*this, inRecoder);
            }
            {
                const auto rasterPassRecoder = inRecoder.BeginRasterPass(Internal::GetRHIRasterPassBeginInfo(*this, inRasterPass->passDesc));
                inRasterPass->passFunc(*this, *rasterPassRecoder);
                rasterPassRecoder->EndPass();
            }
            if (inRasterPass->postPassFunc) {
                inRasterPass->postPassFunc(*this, inRecoder);
            }
        }
        FinalizePassResources(passReadsMap.at(inRasterPass));
        FinalizePassBindGroups(inRasterPass->bindGroups);
    }

    void RGBuilder::PerformBufferUploads()
    {
        bufferUploadTasks.reserve(bufferUploads.size());
        for (const auto& [buffer, uploadInfo] : bufferUploads) {
            DevirtualizeResource(buffer);
            auto* rhiBuffer = GetRHI(buffer);

            bufferUploadTasks.emplace_back(RenderWorkerThreads::Get().EmplaceTask([rhiBuffer, uploadInfo]() -> void {
                const auto* src = static_cast<const uint8_t*>(uploadInfo.data) + uploadInfo.srcOffset;
                auto* dst = rhiBuffer->Map(RHI::MapMode::write, uploadInfo.dstOffset, uploadInfo.size);
                memcpy(dst, src, uploadInfo.size);
                rhiBuffer->UnMap();
            }));
        }
    }

    void RGBuilder::WaitBufferUploadsFinish() const
    {
        for (const auto& task : bufferUploadTasks) {
            task.wait();
        }
    }

    void RGBuilder::DevirtualizeViewsCreatedOnImportedResources()
    {
        for (const auto& view : views) {
            if (!view->GetResource()->imported) {
                continue;
            }

            if (auto* viewRef = view.Get();
                viewRef->Type() == RGResViewType::bufferView) {
                const auto* bufferView = static_cast<RGBufferViewRef>(viewRef);
                auto* buffer = bufferView->GetBuffer();
                devirtualizedResourceViews.emplace(std::make_pair(viewRef, ResourceViewCache::Get(device).GetOrCreate(GetRHI(buffer), bufferView->desc)));
            } else if (viewRef->Type() == RGResViewType::textureView) {
                const auto* textureView = static_cast<RGTextureViewRef>(viewRef);
                auto* texture = textureView->GetTexture();
                devirtualizedResourceViews.emplace(std::make_pair(viewRef, ResourceViewCache::Get(device).GetOrCreate(GetRHI(texture), textureView->desc)));
            } else {
                Unimplement();
            }
        }
    }

    void RGBuilder::DevirtualizeResource(RGResourceRef inResource)
    {
        if (inResource->imported
            || culledResources.contains(inResource)
            || devirtualizedResources.contains(inResource)) {
            return;
        }

        if (inResource->type == RGResType::buffer) {
            devirtualizedResources.emplace(std::make_pair(inResource, BufferPool::Get(device).Allocate(static_cast<RGBufferRef>(inResource)->desc)));
        } else if (inResource->type == RGResType::texture) {
            devirtualizedResources.emplace(std::make_pair(inResource, TexturePool::Get(device).Allocate(static_cast<RGTextureRef>(inResource)->desc)));
        } else {
            Unimplement();
        }
    }

    void RGBuilder::DevirtualizeResources(const std::unordered_set<RGResourceRef>& inResources)
    {
        for (auto* resource : inResources) {
            DevirtualizeResource(resource);
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

                if (item.type == RHI::BindingType::uniformBuffer || item.type == RHI::BindingType::storageBuffer || item.type == RHI::BindingType::rwStorageBuffer) {
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
            devirtualizedBindGroups.emplace(std::make_pair(bindGroup, BindGroupCache::Get(device).Allocate(createInfo)));
        }
    }

    void RGBuilder::DevirtualizeAttachmentViews(const RGRasterPassDesc& inDesc)
    {
        if (inDesc.depthStencilAttachment.has_value()) {
            if (auto* view = inDesc.depthStencilAttachment->view;
                !devirtualizedResourceViews.contains(view)) {
                devirtualizedResourceViews.emplace(std::make_pair(view, ResourceViewCache::Get(device).GetOrCreate(GetRHI(view->GetTexture()), view->desc)));
            }
        }
        for (const auto& colorAttachment : inDesc.colorAttachments) {
            if (auto* view = colorAttachment.view;
                !devirtualizedResourceViews.contains(view)) {
                devirtualizedResourceViews.emplace(std::make_pair(view, ResourceViewCache::Get(device).GetOrCreate(GetRHI(view->GetTexture()), view->desc)));
            }
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
            for (const auto& [type, view] : bindGroup->desc.items | std::views::values) {
                if (type == RHI::BindingType::uniformBuffer) {
                    TransitionBuffer(inRecoder, std::get<RGBufferViewRef>(view)->GetBuffer(), RHI::BufferState::shaderReadOnly);
                } else if (type == RHI::BindingType::storageBuffer) {
                    TransitionBuffer(inRecoder, std::get<RGBufferViewRef>(view)->GetBuffer(), RHI::BufferState::storage);
                } else if (type == RHI::BindingType::rwStorageBuffer) {
                    TransitionBuffer(inRecoder, std::get<RGBufferViewRef>(view)->GetBuffer(), RHI::BufferState::rwStorage);
                } else if (type == RHI::BindingType::texture) {
                    TransitionTexture(inRecoder, std::get<RGTextureViewRef>(view)->GetTexture(), RHI::TextureState::shaderReadOnly);
                } else if (type == RHI::BindingType::storageTexture) {
                    TransitionTexture(inRecoder, std::get<RGTextureViewRef>(view)->GetTexture(), RHI::TextureState::storage);
                } else if (type == RHI::BindingType::sampler) {} else {
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
