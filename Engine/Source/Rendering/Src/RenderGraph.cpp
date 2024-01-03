//
// Created by johnk on 2023/11/28.
//

#include <Rendering/RenderGraph.h>
#include <Common/Container.h>

namespace Rendering {
    RGResourceBase::RGResourceBase(RGResType inType)
        : type(inType)
    {
    }

    RGResourceBase::~RGResourceBase() = default;

    RGResType RGResourceBase::Type() const
    {
        return type;
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

    RGPassType RGPass::Type() const
    {
        return type;
    }

    RGCopyPass::RGCopyPass(std::string inName, RGCopyPassDesc inPassDesc, RGCopyPassExecuteFunc inFunc)
        : RGPass(std::move(inName), RGPassType::copy)
        , passDesc(std::move(inPassDesc))
        , func(std::move(inFunc))
    {
    }

    RGCopyPass::~RGCopyPass() = default;

    void RGCopyPass::Compile()
    {
        for (auto* resource : passDesc.copySrcs) {
            reads.emplace(resource);
        }
        for (auto* resource : passDesc.copyDsts) {
            writes.emplace(resource);
        }
        // TODO
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
        // TODO
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
        // TODO
    }

    RGBuilder::RGBuilder(RHI::Device& inDevice)
        : device(inDevice)
    {
    }

    RGBuilder::~RGBuilder() = default;

    void RGBuilder::Execute(RHI::Fence* mainFence, RHI::Fence* asyncComputeFence, RHI::Fence* asyncCopyFence)
    {
        Compile();
        // TODO
    }

    void RGBuilder::Compile()
    {
        CompilePasses();
        DevirtualizeResources();
    }

    void RGBuilder::CompilePasses()
    {
        for (auto& pass : passes) {
            pass->Compile();
        }
    }

    void RGBuilder::DevirtualizeResources()
    {
        // TODO
    }
}
