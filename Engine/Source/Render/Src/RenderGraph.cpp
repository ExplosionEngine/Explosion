//
// Created by johnk on 2023/2/10.
//

#include <utility>

#include <Render/RenderGraph.h>
#include <Common/Debug.h>

namespace Render {
    RGResource::RGResource(std::string inName, bool inIsExternal)
        : name(std::move(inName))
        , isExternal(inIsExternal)
        , rhiAccess(false)
    {
    }

    RGResource::~RGResource() = default;

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

    void RGFuncCopyPass::Setup(RGPassBuilder& builder)
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
    {
    }

    RGFuncComputePass::~RGFuncComputePass() = default;

    void RGFuncComputePass::Setup(RGPassBuilder& builder)
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

    void RGFuncRasterPass::Setup(RGPassBuilder& builder)
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

    void RenderGraph::Compile()
    {
        // TODO
    }

    void RenderGraph::Setup()
    {
        // TODO
    }

    void RenderGraph::Execute()
    {
        // TODO
    }

    RGPassBuilder::RGPassBuilder(RenderGraph& inGraph, RGPass& inPass)
        : graph(inGraph)
        , pass(inPass)
    {
    }

    RGPassBuilder::~RGPassBuilder() = default;
}
