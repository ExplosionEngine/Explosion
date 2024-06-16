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

    RGBuffer::RGBuffer(RHI::Buffer* inImportedBuffer)
        : RGResource(RGResType::buffer)
        , desc(inImportedBuffer->GetCreateInfo())
        , rhiHandleImported(inImportedBuffer)
    {
        imported = true;
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

    RGTexture::RGTexture(RHI::Texture* inImportedTexture)
        : RGResource(RGResType::texture)
        , desc(inImportedTexture->GetCreateInfo())
        , rhiHandleImported(inImportedTexture)
    {
        imported = true;
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

    RGBufferRef RGBuilder::ImportBuffer(RHI::Buffer* inBuffer)
    {
        Assert(!executed);
        auto* const result = new RGBuffer(inBuffer);
        resources.emplace_back(result);
        return result;
    }

    RGTextureRef RGBuilder::ImportTexture(RHI::Texture* inTexture)
    {
        Assert(!executed);
        auto* const result = new RGTexture(inTexture);
        resources.emplace_back(result);
        return result;
    }

    RGBindGroupRef RGBuilder::AllocateBindGroup(const RGBindGroupDesc& inDesc)
    {
        Assert(!executed);
        bindGroups.emplace_back(new RGBindGroup(inDesc));
        return bindGroups.back().Get();
    }

    void RGBuilder::AddCopyPass(const std::string& inName, const RGCopyPassDesc& inPassDesc, const RGCopyPassExecuteFunc& inFunc)
    {
        Assert(!executed);
        passes.emplace_back(new RGCopyPass(inName, inPassDesc, inFunc));
    }

    void RGBuilder::AddComputePass(const std::string& inName, const std::vector<RGBindGroupRef>& inBindGroups, const RGComputePassExecuteFunc& inFunc)
    {
        Assert(!executed);
        passes.emplace_back(new RGComputePass(inName, inBindGroups, inFunc));
    }

    void RGBuilder::AddRasterPass(const std::string& inName, const RGRasterPassDesc& inPassDesc, const std::vector<RGBindGroupRef>& inBindGroupds, const RGRasterPassExecuteFunc& inFunc)
    {
        Assert(!executed);
        passes.emplace_back(new RGRasterPass(inName, inPassDesc, inBindGroupds, inFunc));
    }

    void RGBuilder::AddSyncPoint()
    {
        Assert(!executed);
        queuePassesVec.emplace_back(queuePassesTemp);
        queuePassesTemp.clear();
    }

    void RGBuilder::Execute(const RHI::Fence& inFence)
    {
        Assert(!executed);
        executed = true;
        Compile();
        ExecuteInternal(inFence);
    }

    RHI::Buffer* RGBuilder::GetRHI(RGBufferRef inBuffer) const
    {
        if (inBuffer->imported) {
            return inBuffer->rhiHandleImported;
        }
        Assert(inBuffer->type == RGResType::buffer);
        return std::get<PooledBufferRef>(devirtualizedResources.at(inBuffer))->GetRHI();
    }

    RHI::Texture* RGBuilder::GetRHI(RGTextureRef inTexture) const
    {
        if (inTexture->imported) {
            return inTexture->rhiHandleImported;
        }
        Assert(inTexture->type == RGResType::texture);
        return std::get<PooledTextureRef>(devirtualizedResources.at(inTexture))->GetRHI();
    }

    RHI::BufferView* RGBuilder::GetRHI(RGBufferViewRef inBufferView) const
    {
        return std::get<RHI::BufferView*>(devirtualizedResourceViews.at(inBufferView));
    }

    RHI::TextureView* RGBuilder::GetRHI(RGTextureViewRef inTextureView) const
    {
        return std::get<RHI::TextureView*>(devirtualizedResourceViews.at(inTextureView));
    }

    RHI::BindGroup* RGBuilder::GetRHI(RGBindGroupRef inBindGroup) const
    {
        return devirtualizedBindGroups.at(inBindGroup).Get();
    }

    void RGBuilder::Compile()
    {
        CompilePassReadWrites();
        PerformCull();
        DevirtualizeResources();
        DevirtualizeBindGroups();
    }

    void RGBuilder::ExecuteInternal(const RHI::Fence& inFence)
    {
        // TODO
    }

    void RGBuilder::CompilePassReadWrites()
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
            resourceReadCounts[resource.Get()] = 0;
        }
        for (const auto& pass : passes) {
            for (auto* read : passReadsMap.at(pass.Get())) {
                resourceReadCounts[read]++;
            }
        }
    }

    void RGBuilder::PerformSyncCheck()
    {
        auto collectQueueReadWrites = [this](const std::vector<RGPassRef>& passes, std::unordered_set<RGResourceRef>& outReads, std::unordered_set<RGResourceRef>& outWrites) -> void {
            for (auto* pass : passes) {
                Common::SetUtils::GetUnionInline(outReads, passReadsMap.at(pass));
                Common::SetUtils::GetUnionInline(outWrites, passWritesMap.at(pass));
            }
        };

        for (const auto& queuePasses : queuePassesVec) {
            // TODO
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
        for (auto riter = queuePassesVec.rbegin(); riter != queuePassesVec.rend(); ++riter) {
            // TODO
        }

        // TODO
    }

    void RGBuilder::DevirtualizeResources()
    {
        // TODO
    }

    void RGBuilder::DevirtualizeBindGroups()
    {
        // TODO
    }
}
