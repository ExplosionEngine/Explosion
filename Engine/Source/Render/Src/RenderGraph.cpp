//
// Created by johnk on 2023/2/10.
//

#include <utility>
#include <unordered_set>
#include <optional>

#include <Render/RenderGraph.h>
#include <Common/Debug.h>

namespace Render {
    static RHI::ComputePassBeginInfo GetRHIComputePassBeginInfo(const RGComputePassDesc& desc)
    {
        RHI::ComputePassBeginInfo result;
        result.pipeline = desc.pipeline;
        return result;
    }

    static std::vector<RHI::GraphicsPassColorAttachment> GetRHIColorAttachments(const RGRasterPassDesc& desc)
    {
        std::vector<RHI::GraphicsPassColorAttachment> result(desc.colorAttachments.size());
        for (auto i = 0; i < result.size(); i++) {
            auto& dst = result[i];
            const auto& src = desc.colorAttachments[i];

            dst.view = src.view == nullptr ? nullptr : src.view->GetRHI();
            dst.resolve = src.resolve == nullptr ? nullptr : src.resolve->GetRHI();
            dst.clearValue = src.clearValue;
            dst.loadOp = src.loadOp;
            dst.storeOp = src.storeOp;
        }
        return result;
    }

    static std::optional<RHI::GraphicsPassDepthStencilAttachment> GetRHIDepthStencilAttachment(const RGRasterPassDesc& desc)
    {
        if (!desc.depthStencilAttachment.has_value()) {
            return {};
        }

        const auto& src = desc.depthStencilAttachment.value();

        RHI::GraphicsPassDepthStencilAttachment result;
        result.view = src.view == nullptr ? nullptr : src.view->GetRHI();
        result.depthClearValue = src.depthClearValue;
        result.depthLoadOp = src.depthLoadOp;
        result.depthStoreOp = src.depthStoreOp;
        result.depthReadOnly = src.depthReadOnly;
        result.stencilClearValue = src.stencilClearValue;
        result.stencilLoadOp = src.stencilLoadOp;
        result.stencilStoreOp = src.stencilStoreOp;
        result.stencilReadOnly = src.stencilReadOnly;
        return result;
    }

    static RHI::GraphicsPassBeginInfo GetRHIGraphicsPassBeginInfo(
        const RGRasterPassDesc& desc,
        uint32_t colorAttachmentNum,
        const RHI::GraphicsPassColorAttachment* colorAttachments,
        const RHI::GraphicsPassDepthStencilAttachment* depthStencilAttachment)
    {
        RHI::GraphicsPassBeginInfo result;
        result.pipeline = desc.pipeline;
        result.colorAttachmentNum = colorAttachmentNum;
        result.colorAttachments = colorAttachments;
        result.depthStencilAttachment = depthStencilAttachment;
        return result;
    }
}

namespace Render {
    RGResource::RGResource(std::string inName, bool inIsExternal)
        : name(std::move(inName))
        , isExternal(inIsExternal)
        , rhiAccess(false)
    {
    }

    RGResource::~RGResource() = default;

    void RGResource::SetCulled(bool inCulled)
    {
        isCulled = inCulled;
    }

    void RGResource::SetRHIAccess(bool inRhiAccess)
    {
        rhiAccess = inRhiAccess;
    }

    RGBuffer::RGBuffer(std::string inName, RGBufferDesc inDesc)
        : RGResource(std::move(inName), false)
        , desc(inDesc)
    {
    }

    RGBuffer::RGBuffer(std::string inName, RHI::Buffer* inBuffer)
        : RGResource(std::move(inName), true)
        , rhiHandle(inBuffer)
    {
    }

    RGBuffer::~RGBuffer() = default;

    RGResourceType RGBuffer::GetType()
    {
        return RGResourceType::BUFFER;
    }

    void RGBuffer::Devirtualize()
    {
        // TODO
    }

    void RGBuffer::Destroy()
    {
        if (!isExternal && rhiHandle != nullptr) {
            rhiHandle->Destroy();
        }
    }

    RHI::Buffer* RGBuffer::GetRHI()
    {
        Assert(rhiAccess);
        return rhiHandle;
    }

    RGTexture::RGTexture(std::string inName, RGTextureDesc inDesc)
        : RGResource(std::move(inName), false)
        , desc(inDesc)
    {
    }

    RGTexture::RGTexture(std::string inName, RHI::Texture* inTexture)
        : RGResource(std::move(inName), true)
        , rhiHandle(inTexture)
    {
    }

    RGTexture::~RGTexture() = default;

    RGResourceType RGTexture::GetType()
    {
        return RGResourceType::TEXTURE;
    }

    void RGTexture::Devirtualize()
    {
        // TODO
    }

    void RGTexture::Destroy()
    {
        if (!isExternal && rhiHandle != nullptr) {
            rhiHandle->Destroy();
        }
    }

    RHI::Texture* RGTexture::GetRHI()
    {
        Assert(rhiAccess);
        return rhiHandle;
    }

    RGBufferView::RGBufferView(std::string inName, RGBuffer* inBuffer, RGBufferViewDesc inDesc)
        : RGResource(std::move(inName), false)
        , buffer(inBuffer)
        , desc(inDesc)
    {
    }

    RGBufferView::RGBufferView(std::string inName, RHI::BufferView* inBufferView)
        : RGResource(std::move(inName), true)
        , rhiHandle(inBufferView)
    {
    }

    RGBufferView::~RGBufferView() = default;

    RGResourceType RGBufferView::GetType()
    {
        return RGResourceType::BUFFER_VIEW;
    }

    void RGBufferView::Devirtualize()
    {
        // TODO
    }

    void RGBufferView::Destroy()
    {
        if (!isExternal && rhiHandle != nullptr) {
            rhiHandle->Destroy();
        }
    }

    RHI::BufferView* RGBufferView::GetRHI()
    {
        Assert(rhiAccess);
        return rhiHandle;
    }

    RGBuffer* RGBufferView::GetBuffer()
    {
        Assert(!isExternal);
        return buffer;
    }

    RGTextureView::RGTextureView(std::string inName, RGTexture* inTexture, RGTextureViewDesc inDesc)
        : RGResource(std::move(inName), false)
        , texture(inTexture)
        , desc(inDesc)
    {
    }

    RGTextureView::RGTextureView(std::string inName, RHI::TextureView* inTextureView)
        : RGResource(std::move(inName), true)
        , rhiHandle(inTextureView)
    {
    }

    RGTextureView::~RGTextureView() = default;

    RGResourceType RGTextureView::GetType()
    {
        return RGResourceType::TEXTURE_VIEW;
    }

    void RGTextureView::Devirtualize()
    {
        // TODO
    }

    void RGTextureView::Destroy()
    {
        if (!isExternal && rhiHandle != nullptr) {
            rhiHandle->Destroy();
        }
    }

    RHI::TextureView* RGTextureView::GetRHI()
    {
        Assert(rhiAccess);
        return rhiHandle;
    }

    RGTexture* RGTextureView::GetTexture()
    {
        Assert(!isExternal);
        return texture;
    }

    RGSampler::RGSampler(std::string inName, RGSamplerDesc inDesc)
        : RGResource(std::move(inName), false)
        , desc(inDesc)
    {
    }

    RGSampler::RGSampler(std::string inName, RHI::Sampler* inSampler)
        : RGResource(std::move(inName), true)
        , rhiHandle(inSampler)
    {
    }

    RGSampler::~RGSampler() = default;

    RGResourceType RGSampler::GetType()
    {
        return RGResourceType::SAMPLER;
    }

    void RGSampler::Devirtualize()
    {
        // TODO
    }

    void RGSampler::Destroy()
    {
        if (!isExternal && rhiHandle != nullptr) {
            rhiHandle->Destroy();
        }
    }

    RHI::Sampler* RGSampler::GetRHI()
    {
        Assert(rhiAccess);
        return rhiHandle;
    }

    RGPass::RGPass(std::string inName)
        : name(std::move(inName))
    {
    }

    RGPass::~RGPass() = default;

    RGCopyPass::RGCopyPass(std::string inName)
        : RGPass(std::move(inName))
    {
    }

    RGCopyPass::~RGCopyPass() = default;

    RGPassType RGCopyPass::GetType()
    {
        return RGPassType::COPY;
    }

    RGComputePass::RGComputePass(std::string inName)
        : RGPass(std::move(inName))
    {
    }

    RGComputePass::~RGComputePass() = default;

    RGPassType RGComputePass::GetType()
    {
        return RGPassType::COMPUTE;
    }

    RGRasterPass::RGRasterPass(std::string inName)
        : RGPass(std::move(inName))
    {
    }

    RGRasterPass::~RGRasterPass() = default;

    RGPassType RGRasterPass::GetType()
    {
        return RGPassType::RASTER;
    }

    RGFuncCopyPass::RGFuncCopyPass(std::string inName, RGCopyPassSetupFunc inSetupFunc)
        : RGCopyPass(std::move(inName))
        , setupFunc(std::move(inSetupFunc))
    {
    }

    RGFuncCopyPass::~RGFuncCopyPass() = default;

    void RGFuncCopyPass::Setup(RGCopyPassBuilder& builder)
    {
        executeFunc = setupFunc(builder);
    }

    void RGFuncCopyPass::Execute(RHI::CommandEncoder& encoder)
    {
        Assert(executeFunc != nullptr);
        executeFunc(encoder);
    }

    RGFuncComputePass::RGFuncComputePass(std::string inName, RGComputePassSetupFunc inSetupFunc)
        : RGComputePass(std::move(inName))
        , setupFunc(std::move(inSetupFunc))
        , executeFunc(nullptr)
    {
    }

    RGFuncComputePass::~RGFuncComputePass() = default;

    void RGFuncComputePass::Setup(RGComputePassBuilder& builder)
    {
        executeFunc = setupFunc(builder);
    }

    void RGFuncComputePass::Execute(RHI::ComputePassCommandEncoder& encoder)
    {
        Assert(executeFunc != nullptr);
        executeFunc(encoder);
    }

    RGFuncRasterPass::RGFuncRasterPass(std::string inName, RGRasterPassSetupFunc inSetupFunc)
        : RGRasterPass(std::move(inName))
        , setupFunc(std::move(inSetupFunc))
    {
    }

    RGFuncRasterPass::~RGFuncRasterPass() = default;

    void RGFuncRasterPass::Setup(RGRasterPassBuilder& builder)
    {
        executeFunc = setupFunc(builder);
    }

    void RGFuncRasterPass::Execute(RHI::GraphicsPassCommandEncoder& encoder)
    {
        Assert(executeFunc != nullptr);
        executeFunc(encoder);
    }

    RenderGraph::RenderGraph(RHI::Device& inDevice)
        : device(inDevice)
    {
    }

    RenderGraph::~RenderGraph() = default;

    void RenderGraph::AddCopyPass(RGCopyPass* inPass)
    {
        passes.emplace_back(inPass);
    }

    void RenderGraph::AddComputePass(RGComputePass* inPass)
    {
        passes.emplace_back(inPass);
    }

    void RenderGraph::AddRasterPass(RGRasterPass* inPass)
    {
        passes.emplace_back(inPass);
    }

    void RenderGraph::AddCopyPass(std::string inName, RGCopyPassSetupFunc inSetupFunc)
    {
        passes.emplace_back(new RGFuncCopyPass(std::move(inName), std::move(inSetupFunc)));
    }

    void RenderGraph::AddComputePass(std::string inName, RGComputePassSetupFunc inSetupFunc)
    {
        passes.emplace_back(new RGFuncComputePass(std::move(inName), std::move(inSetupFunc)));
    }

    void RenderGraph::AddRasterPass(std::string inName, RGRasterPassSetupFunc inSetupFunc)
    {
        passes.emplace_back(new RGFuncRasterPass(std::move(inName), std::move(inSetupFunc)));
    }

    void RenderGraph::Setup()
    {
        for (auto& pass : passes) {
            if (pass->GetType() == RGPassType::COPY) {
                auto* copyPass = static_cast<RGCopyPass*>(pass.get());
                RGCopyPassBuilder builder(*this, *copyPass);
                copyPass->Setup(builder);
            } else if (pass->GetType() == RGPassType::COMPUTE) {
                auto* computePass = static_cast<RGComputePass*>(pass.get());
                RGComputePassBuilder builder(*this, *computePass);
                computePass->Setup(builder);
            } else if (pass->GetType() == RGPassType::RASTER) {
                auto* rasterPass = static_cast<RGRasterPass*>(pass.get());
                RGRasterPassBuilder builder(*this, *rasterPass);
                rasterPass->Setup(builder);
            } else {
                Assert(false);
            }
        }
    }

    void RenderGraph::Compile()
    {
        for (auto& resource : resources) {
            resource->Destroy();
            resource->SetCulled(false);
        }

        std::unordered_set<RGResource*> consumeds;
        for (auto& pass : passes) {
            for (auto& read : pass->reads) {
                consumeds.emplace(read);
            }
        }

        for (auto& resource : resources) {
            if (!consumeds.contains(resource.get())) {
                resource->SetCulled(true);
            }

            if (resource->isCulled) {
                continue;
            }
            for (auto& pass : passes) {
                auto iter = std::find(pass->writes.begin(), pass->writes.end(), resource.get());
                if (iter == pass->writes.end()) {
                    continue;
                }
                writes[resource.get()] = pass.get();
                break;
            }
        }
    }

    void RenderGraph::Execute(RHI::Fence* mainFence, RHI::Fence* asyncFence)
    {
        for (auto& resource : resources) {
            if (!resource->isCulled) {
                resource->Devirtualize();
                resource->SetRHIAccess(true);
            }
        }

        Assert(device.GetQueueNum(RHI::QueueType::GRAPHICS) > 0);
        RHI::Queue* mainQueue = device.GetQueue(RHI::QueueType::GRAPHICS, 0);
        RHI::Queue* asyncComputeQueue = device.GetQueueNum(RHI::QueueType::COMPUTE) > 1 ? device.GetQueue(RHI::QueueType::COMPUTE, 1) : mainQueue;

        RHI::CommandBuffer* mainBuffer = device.CreateCommandBuffer();
        RHI::CommandBuffer* asyncComputeBuffer = device.CreateCommandBuffer();
        RHI::CommandEncoder* mainEncoder = mainBuffer->Begin();
        RHI::CommandEncoder* asyncComputeEncoder = asyncComputeBuffer->Begin();
        {
            for (auto& pass : passes) {
                if (pass->GetType() == RGPassType::COPY) {
                    auto* copyPass = static_cast<RGCopyPass*>(pass.get());
                    TransitionResources(mainEncoder, copyPass);
                    ExecuteCopyPass(mainEncoder, copyPass);
                } else if (pass->GetType() == RGPassType::RASTER) {
                    auto* computePass = static_cast<RGComputePass*>(pass.get());
                    RHI::CommandEncoder* commandEncoder = computePass->isAsyncCompute ? asyncComputeEncoder : mainEncoder;
                    TransitionResources(commandEncoder, computePass);
                    ExecuteComputePass(commandEncoder, computePass);
                } else if (pass->GetType() == RGPassType::COMPUTE) {
                    auto* rasterPass = static_cast<RGRasterPass*>(pass.get());
                    TransitionResources(mainEncoder, rasterPass);
                    ExecuteRasterPass(mainEncoder, rasterPass);
                } else {
                    Assert(false);
                }
            }
        }
        mainEncoder->End();

        mainQueue->Submit(mainBuffer, mainFence);
        asyncComputeQueue->Submit(asyncComputeBuffer, asyncFence);
        mainBuffer->Destroy();
        asyncComputeBuffer->Destroy();
    }

    void RenderGraph::ExecuteCopyPass(RHI::CommandEncoder* encoder, RGCopyPass* copyPass)
    {
        copyPass->Execute(*encoder);
    }

    void RenderGraph::ExecuteComputePass(RHI::CommandEncoder* encoder, RGComputePass* computePass)
    {
        RHI::ComputePassBeginInfo beginInfo = GetRHIComputePassBeginInfo(computePass->passDesc);
        RHI::ComputePassCommandEncoder* computeEncoder = encoder->BeginComputePass(&beginInfo);
        {
            computePass->Execute(*computeEncoder);
        }
        computeEncoder->EndPass();
    }

    void RenderGraph::ExecuteRasterPass(RHI::CommandEncoder* encoder, RGRasterPass* rasterPass)
    {
        std::vector<RHI::GraphicsPassColorAttachment> colorAttachments = GetRHIColorAttachments(rasterPass->passDesc);
        std::optional<RHI::GraphicsPassDepthStencilAttachment> depthStencilAttachment = GetRHIDepthStencilAttachment(rasterPass->passDesc);
        RHI::GraphicsPassBeginInfo beginInfo = GetRHIGraphicsPassBeginInfo(
            rasterPass->passDesc,
            colorAttachments.size(),
            colorAttachments.data(),
            depthStencilAttachment.has_value() ? &depthStencilAttachment.value() : nullptr);
        RHI::GraphicsPassCommandEncoder* graphicsEncoder = encoder->BeginGraphicsPass(&beginInfo);
        {
            rasterPass->Execute(*graphicsEncoder);
        }
        graphicsEncoder->EndPass();
    }

    void RenderGraph::TransitionResources(RHI::CommandEncoder* encoder, RGPass* readPass)
    {
        auto& readResources = readPass->reads;
        for (auto* readResource : readResources) {
            auto iter = writes.find(readResource);
            auto* writePass = iter == writes.end() ? nullptr : iter->second;
            TransitionSingleResource(encoder, readResource, writePass, readPass);
        }
    }

    void RenderGraph::TransitionSingleResource(RHI::CommandEncoder* encoder, RGResource* resource, RGPass* writePass, RGPass* readPass)
    {
        auto type = resource->GetType();
        if (type == RGResourceType::BUFFER) {
            TransitionSingleBuffer(encoder, static_cast<RGBuffer*>(resource), writePass, readPass);
        } else if (type == RGResourceType::BUFFER_VIEW) {
            TransitionSingleBufferView(encoder, static_cast<RGBufferView*>(resource), writePass, readPass);
        } else if (type == RGResourceType::TEXTURE) {
            TransitionSingleTexture(encoder, static_cast<RGTexture*>(resource), writePass, readPass);
        } else if (type == RGResourceType::TEXTURE_VIEW) {
            TransitionSingleTextureView(encoder, static_cast<RGTextureView*>(resource), writePass, readPass);
        } else {
            Assert(false);
        }
    }

    RHI::BufferState RenderGraph::SpeculateBufferStateFrom(RGPass* writePass)
    {
        // TODO
        return RHI::BufferState::MAX;
    }

    RHI::BufferState RenderGraph::SpeculateBufferStateTo(RGPass* readPass)
    {
        // TODO
        return RHI::BufferState::MAX;
    }

    RHI::TextureState RenderGraph::SpeculateTextureStateFrom(RGPass* writePass)
    {
        // TODO
        return RHI::TextureState::PRESENT;
    }

    RHI::TextureState RenderGraph::SpeculateTextureStateTo(RGPass* readPass)
    {
        // TODO
        return RHI::TextureState::PRESENT;
    }

    RGPassBuilder::RGPassBuilder(RenderGraph& inGraph, RGPass& inPass)
        : graph(inGraph)
        , pass(inPass)
    {
    }

    RGPassBuilder::~RGPassBuilder() = default;

    RGCopyPassBuilder::RGCopyPassBuilder(RenderGraph& inGraph, RGCopyPass& inPass)
        : RGPassBuilder(inGraph, inPass)
        , copyPass(inPass)
    {
    }

    RGCopyPassBuilder::~RGCopyPassBuilder() = default;

    RGComputePassBuilder::RGComputePassBuilder(RenderGraph& inGraph, RGComputePass& inPass)
        : RGPassBuilder(inGraph, inPass)
        , computePass(inPass)
    {
    }

    RGComputePassBuilder::~RGComputePassBuilder() = default;

    void RGComputePassBuilder::SetAsyncCompute(bool inAsyncCompute)
    {
        computePass.isAsyncCompute = inAsyncCompute;
    }

    void RGComputePassBuilder::SetPassDesc(const RGComputePassDesc& inDesc)
    {
        computePass.passDesc = inDesc;
    }

    RGRasterPassBuilder::RGRasterPassBuilder(RenderGraph& inGraph, RGRasterPass& inPass)
        : RGPassBuilder(inGraph, inPass)
        , rasterPass(inPass)
    {
    }

    RGRasterPassBuilder::~RGRasterPassBuilder() = default;

    void RGRasterPassBuilder::SetPassDesc(const RGRasterPassDesc& inDesc)
    {
        rasterPass.passDesc = inDesc;
    }
}
