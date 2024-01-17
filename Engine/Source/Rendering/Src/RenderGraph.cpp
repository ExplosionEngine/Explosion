//
// Created by johnk on 2023/11/28.
//

#include <ranges>

#include <Rendering/RenderGraph.h>
#include <Common/Container.h>

namespace Rendering::Internal {
    static void CompilePassForBindGroups(
        std::unordered_set<RGResourceRef>& reads,
        std::unordered_set<RGResourceRef>& writes,
        const std::vector<RGBindGroupRef>& bindGroups)
    {
        auto checkAndMark = [](std::unordered_set<RGResourceRef>& marks, RGResourceViewRef view) -> void {
            RGResourceRef resource = view->GetResourceBase();
            Assert(!marks.contains(resource));
            marks.emplace(resource);
        };
        auto predicateActions = std::unordered_map<RHI::BindingType, std::function<void(const RGBindItemDesc&)>> {
            { RHI::BindingType::uniformBuffer, [&](const RGBindItemDesc& desc) -> void { checkAndMark(reads, desc.bufferView); } },
            { RHI::BindingType::storageBuffer, [&](const RGBindItemDesc& desc) -> void { checkAndMark(writes, desc.bufferView); } },
            { RHI::BindingType::texture, [&](const RGBindItemDesc& desc) -> void { checkAndMark(reads, desc.textureView); } },
            { RHI::BindingType::storageTexture, [&](const RGBindItemDesc& desc) -> void { checkAndMark(writes, desc.textureView); } }
        };

        for (const auto* bindGroup : bindGroups) {
            const auto& bindGroupDesc = bindGroup->GetDesc();
            const auto& bindItems = bindGroupDesc.items;

            for (const auto& itemDesc : bindItems | std::views::values) {
                predicateActions.at(itemDesc.type)(itemDesc);
            }
        }
    }

    static RHI::GraphicsPassDepthStencilAttachment GetRasterPassDepthStencilAttachment(const RGRasterPassDesc& desc)
    {
        static_assert(std::is_base_of_v<RHI::GraphicsPassDepthStencilAttachmentBase, RGDepthStencilAttachment>);

        RHI::GraphicsPassDepthStencilAttachment result;
        if (desc.depthStencilAttachment.has_value()) {
            memcpy(&result, &desc.depthStencilAttachment.value(), sizeof(RHI::GraphicsPassDepthStencilAttachmentBase));
            result.view = desc.depthStencilAttachment->view->GetRHI();
        }
        return result;
    }

    static std::vector<RHI::GraphicsPassColorAttachment> GetRasterPassColorAttachments(const RGRasterPassDesc& desc)
    {
        static_assert(std::is_base_of_v<RHI::GraphicsPassColorAttachmentBase, RGColorAttachment>);

        std::vector<RHI::GraphicsPassColorAttachment> result;
        result.reserve(desc.colorAttachments.size());

        for (const auto& colorAttachment : desc.colorAttachments) {
            RHI::GraphicsPassColorAttachment back;
            memcpy(&back, &colorAttachment, sizeof(RHI::GraphicsPassColorAttachmentBase));
            back.view = colorAttachment.view->GetRHI();
            result.emplace_back(std::move(back));
        }
        return result;
    }

    class CommandBuffersGuard {
    public:
        struct Context {
            RHI::CommandBuffer* mainCmdBuffer;
            RHI::CommandBuffer* asyncCopyCmdBuffer;
            RHI::CommandBuffer* asyncComputeCmdBuffer;
        };

        CommandBuffersGuard(RHI::Device& inDevice, bool inHasAsyncCopy, bool inHasAsyncCompute, const RGFencePack& inFencePack, std::function<void(const Context&)>&& inAction)
            : device(inDevice)
            , fencePack(inFencePack)
        {
            const bool allowAsyncCopy = device.GetQueueNum(RHI::QueueType::transfer) > 1;
            const bool allowAsyncCompute = device.GetQueueNum(RHI::QueueType::compute) > 1;
            useAsyncCopy = inHasAsyncCopy && allowAsyncCopy;
            useAsyncCompute = inHasAsyncCompute && allowAsyncCompute;

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

        ~CommandBuffersGuard()
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

            device.GetQueue(RHI::QueueType::graphics, 0)->Submit(mainCmdBuffer.Get(), fencePack.mainFence);
            if (useAsyncCopy) {
                device.GetQueue(RHI::QueueType::transfer, 1)->Submit(asyncCopyCmdBuffer.Get(), fencePack.asyncCopyFence);
            }
            if (useAsyncCompute) {
                device.GetQueue(RHI::QueueType::compute, 1)->Submit(asyncComputeCmdBuffer.Get(), fencePack.asyncComputeFence);
            }
        }

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
    RGResourceBase::RGResourceBase(RGResType inType)
        : type(inType)
        , forceUsed(false)
        , imported(true)
    {
    }

    RGResourceBase::~RGResourceBase() = default;

    RGResType RGResourceBase::Type() const
    {
        return type;
    }

    void RGResourceBase::MaskAsUsed()
    {
        forceUsed = true;
    }

    RGResourceViewBase::RGResourceViewBase(Rendering::RGResViewType inType)
        : type(inType)
    {
    }

    RGResourceViewBase::~RGResourceViewBase() = default;

    RGResViewType RGResourceViewBase::Type() const
    {
        return type;
    }

    RGBindGroupDesc RGBindGroupDesc::Create(RHI::BindGroupLayout* inLayout)
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
        : desc(std::move(inDesc))
    {
    }

    RGBindGroup::~RGBindGroup() = default;

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
          , writes()
    {
    }

    RGPass::~RGPass() = default;

    RGCopyPass::RGCopyPass(std::string inName, RGCopyPassDesc inPassDesc, RGCopyPassExecuteFunc inFunc, bool inAsyncCopy)
        : RGPass(std::move(inName), RGPassType::copy)
        , passDesc(std::move(inPassDesc))
        , func(std::move(inFunc))
        , asyncCopy(inAsyncCopy)
    {
    }

    RGCopyPass::~RGCopyPass() = default;

    void RGCopyPass::Compile()
    {
        for (auto* resource: passDesc.copySrcs) {
            Assert(!reads.contains(resource));
            reads.emplace(resource);
        }
        for (auto* resource: passDesc.copyDsts) {
            Assert(writes.contains(resource));
            writes.emplace(resource);
        }
        Assert(Common::SetUtils::GetIntersection(reads, writes).size() == 0);
    }

    RGComputePass::RGComputePass(std::string inName, std::vector<RGBindGroupRef> inBindGroups, RGComputePassExecuteFunc inFunc, bool inAsyncCompute)
        : RGPass(std::move(inName), RGPassType::compute)
        , asyncCompute(inAsyncCompute)
        , bindGroups(std::move(inBindGroups))
        , func(std::move(inFunc))
    {
    }

    RGComputePass::~RGComputePass() = default;

    void RGComputePass::Compile()
    {
        Internal::CompilePassForBindGroups(reads, writes, bindGroups);
    }

    RGRasterPass::RGRasterPass(std::string inName, RGRasterPassDesc inPassDesc, std::vector<RGBindGroupRef> inBindGroupds, RGRasterPassExecuteFunc inFunc)
        : RGPass(std::move(inName), RGPassType::raster)
        , passDesc(std::move(inPassDesc))
        , bindGroups(std::move(inBindGroupds))
        , func(std::move(inFunc))
    {
    }

    RGRasterPass::~RGRasterPass() = default;

    void RGRasterPass::Compile()
    {
        auto checkAndMarkWrite = [&](RGResourceViewRef view) -> void
        {
            RGResourceRef resource = view->GetResourceBase();
            Assert(!writes.contains(resource));
            writes.emplace(resource);
        };

        for (const auto& colorAttachment: passDesc.colorAttachments) {
            checkAndMarkWrite(colorAttachment.view);
        }
        if (passDesc.depthStencilAttachment.has_value()) {
            checkAndMarkWrite(passDesc.depthStencilAttachment->view);
        }
        Internal::CompilePassForBindGroups(reads, writes, bindGroups);
    }

    RGFencePack::RGFencePack() = default;

    RGFencePack::~RGFencePack() = default;

    RGFencePack::RGFencePack(RHI::Fence* inMainFence, RHI::Fence* inAsyncComputeFence, RHI::Fence* inAsyncCopyFence)
        : mainFence(inMainFence)
        , asyncComputeFence(inAsyncComputeFence)
        , asyncCopyFence(inAsyncCopyFence)
    {
    }

    RGBuilder::RGBuilder(RHI::Device& inDevice)
        : device(inDevice)
        , executed(false)
    {
    }

    RGBuilder::~RGBuilder() = default;

    RGBindGroupRef RGBuilder::AllocateBindGroup(const RGBindGroupDesc& inDesc)
    {
        bindGroups.emplace_back(Common::UniqueRef<RGBindGroup>(new RGBindGroup(inDesc)));
        return bindGroups.back().Get();
    }

    void RGBuilder::Execute(const RGFencePack& inFencePack)
    {
        Assert(!executed);
        ExecuteContext context = Compile();
        ExecuteInternal(context, inFencePack);
        FinalizeResources(context);
        executed = true;
    }

    RGBuilder::ExecuteContext RGBuilder::Compile()
    {
        ExecuteContext context = ExecuteContext {
            .hasAsyncCopy = false,
            .hasAsyncCompute = false,
            .devirtualizedResources = {},
            .resourceStates = {},
        };

        CompilePasses(context);
        DevirtualizeResources(context);
        return context;
    }

    void RGBuilder::CompilePasses(RGBuilder::ExecuteContext& context)
    {
        for (auto& pass : passes) {
            pass->Compile();
        }

        for (auto& pass : passes) {
            if (pass->type == RGPassType::copy) {
                context.hasAsyncCopy = context.hasAsyncCopy || static_cast<RGCopyPass*>(pass.Get())->asyncCopy;
            } else if (pass->type == RGPassType::compute) {
                context.hasAsyncCompute = context.hasAsyncCompute || static_cast<RGComputePass*>(pass.Get())->asyncCompute;
            }
        }
    }

    void RGBuilder::DevirtualizeResources(RGBuilder::ExecuteContext& context)
    {
        auto allocateResourcesOrUpdateRefCount = [&](RGResourceRef resource) -> void {
            if (resource->imported) {
                return;
            }

            auto iter = context.devirtualizedResources.find(resource);
            if (iter == context.devirtualizedResources.end()) {
                if (resource->type == RGResType::buffer) {
                    auto* bufferRef = static_cast<RGBufferRef>(resource);
                    context.devirtualizedResources.emplace(std::make_pair(
                        resource,
                        std::make_pair(std::variant<PooledBufferRef, PooledTextureRef>(BufferPool::Get(device).Allocate(bufferRef->desc)), 1)
                    ));
                    bufferRef->rhiHandle = std::get<PooledBufferRef>(context.devirtualizedResources.at(resource).first)->GetRHI();
                } else if (resource->type == RGResType::texture) {
                    auto* textureRef = static_cast<RGTextureRef>(resource);
                    context.devirtualizedResources.emplace(std::make_pair(
                        resource,
                        std::make_pair(std::variant<PooledBufferRef, PooledTextureRef>(TexturePool ::Get(device).Allocate(textureRef->desc)), 1)
                    ));
                    textureRef->rhiHandle = std::get<PooledTextureRef>(context.devirtualizedResources.at(resource).first)->GetRHI();
                } else {
                    Unimplement();
                }
            } else {
                iter->second.second++;
            }
        };

        context.devirtualizedResources.clear();
        for (const auto& resource : resources) {
            if (resource->forceUsed) {
                allocateResourcesOrUpdateRefCount(resource.Get());
            }
        }
        for (const auto& pass : passes) {
            for (auto* read : pass->reads) {
                allocateResourcesOrUpdateRefCount(read);
            }
        }
    }

    void RGBuilder::ExecuteInternal(RGBuilder::ExecuteContext& executeContext, const Rendering::RGFencePack& inFencePack)
    {
        Internal::CommandBuffersGuard guard(device, executeContext.hasAsyncCopy, executeContext.hasAsyncCompute, inFencePack, [&](const Internal::CommandBuffersGuard::Context& guardContext) -> void {
            for (auto& pass : passes) {
                if (pass->type == RGPassType::copy) {
                    RGCopyPass& copyPass = *static_cast<RGCopyPass*>(pass.Get());
                    RHI::CommandBuffer* cmdBuffer = copyPass.asyncCopy ? guardContext.asyncCopyCmdBuffer : guardContext.mainCmdBuffer;
                    Common::UniqueRef<RHI::CommandEncoder> encoder = cmdBuffer->Begin();
                    {
                        Common::UniqueRef<RHI::CopyPassCommandEncoder> copyPassEncoder = encoder->BeginCopyPass();
                        ExecutePass(executeContext, copyPassEncoder.Get(), copyPass);
                        copyPassEncoder->EndPass();
                    }
                    encoder->End();
                } else if (pass->type == RGPassType::compute) {
                    RGComputePass& computePass = *static_cast<RGComputePass*>(pass.Get());
                    RHI::CommandBuffer* cmdBuffer = computePass.asyncCompute ? guardContext.asyncComputeCmdBuffer : guardContext.mainCmdBuffer;
                    Common::UniqueRef<RHI::CommandEncoder> encoder = cmdBuffer->Begin();
                    {
                        Common::UniqueRef<RHI::ComputePassCommandEncoder> computePassEncoder = encoder->BeginComputePass();
                        ExecutePass(executeContext, computePassEncoder.Get(), computePass);
                        computePassEncoder->EndPass();
                    }
                    encoder->End();
                } else if (pass->type == RGPassType::raster) {
                    RGRasterPass& rasterPass = *static_cast<RGRasterPass*>(pass.Get());
                    RHI::CommandBuffer* cmdBuffer = guardContext.mainCmdBuffer;
                    Common::UniqueRef<RHI::CommandEncoder> encoder = cmdBuffer->Begin();
                    {
                        Common::UniqueRef<RHI::GraphicsPassCommandEncoder> rasterPassEncoder = encoder->BeginGraphicsPass();
                        ExecutePass(executeContext, rasterPassEncoder.Get(), rasterPass);
                        rasterPassEncoder->EndPass();
                    }
                    encoder->End();
                } else {
                    Unimplement();
                }

                for (auto& read : pass->reads) {
                    DecRefCountAndFinalizeResourceIfNeeded(executeContext, read);
                }
            }
        });
    }

    void RGBuilder::DecRefCountAndFinalizeResourceIfNeeded(ExecuteContext& context, RGResourceRef resource)
    {
        auto& pooledResourcesAndRefCount = context.devirtualizedResources.at(resource);
        Assert(pooledResourcesAndRefCount.second > 0);
        pooledResourcesAndRefCount.second--;

        if (pooledResourcesAndRefCount.second == 0) {
            FinalizeResource(context, resource);
        }
    }

    void RGBuilder::FinalizeResource(RGBuilder::ExecuteContext& context, RGResourceRef resource)
    {
        auto& pooledResourcesAndRefCount = context.devirtualizedResources.at(resource);
        Assert(pooledResourcesAndRefCount.second == 0);

        if (resource->type == RGResType::buffer) {
            auto* bufferRef = static_cast<RGBufferRef>(resource);
            bufferRef->rhiHandle = nullptr;
        } else if (resource->type == RGResType::texture) {
            auto* textureRef = static_cast<RGTextureRef>(resource);
            textureRef->rhiHandle = nullptr;
        } else {
            Unimplement();
        }
        context.devirtualizedResources.erase(resource);
    }

    void RGBuilder::FinalizeResources(RGBuilder::ExecuteContext& context)
    {
        for (auto& iter : context.devirtualizedResources) {
            auto* resource = iter.first;
            if (resource->type == RGResType::buffer) {
                auto* bufferRef = static_cast<RGBufferRef>(resource);
                bufferRef->rhiHandle = nullptr;
            } else if (resource->type == RGResType::texture) {
                auto* textureRef = static_cast<RGTextureRef>(resource);
                textureRef->rhiHandle = nullptr;
            } else {
                Unimplement();
            }
        }
        context.devirtualizedResources.clear();
    }
}
