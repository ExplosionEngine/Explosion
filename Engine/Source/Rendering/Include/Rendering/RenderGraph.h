//
// Created by johnk on 2023/11/28.
//

#pragma once

#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <functional>

#include <Common/Memory.h>
#include <Common/Debug.h>
#include <RHI/RHI.h>

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

    using RGBufferDesc = RHI::BufferCreateInfo;
    using RGTextureDesc = RHI::TextureCreateInfo;

    class RGResourceBase {
    public:
        virtual ~RGResourceBase();

        RGResType Type() const;

    protected:
        explicit RGResourceBase(RGResType inType);

        RGResType type;
    };

    template <typename RHIResource>
    struct RHIResourceTraits {};

    template <typename RHIResource>
    class RGResource : public RGResourceBase {
    public:
        using Desc = typename RHIResourceTraits<RHIResource>::DescType;

        ~RGResource() override;

        const Desc& GetDesc() const;
        RHIResource* GetRHI() const;

    private:
        friend class RGBuilder;

        explicit RGResource(Desc inDesc);

        Desc desc;
        RHIResource* rhiHandle;
    };

    using RGBuffer = RGResource<RHI::Buffer>;
    using RGTexture = RGResource<RHI::Texture>;
    using RGResourceRef = RGResourceBase*;
    using RGBufferRef = RGBuffer*;
    using RGTextureRef = RGTexture*;

    using RGBufferViewDesc = RHI::BufferViewCreateInfo;
    using RGTextureViewDesc = RHI::TextureViewCreateInfo;

    class RGResourceViewBase {
    public:
        virtual ~RGResourceViewBase();

        RGResViewType Type() const;
        virtual RGResourceRef GetResourceBase() = 0;

    protected:
        explicit RGResourceViewBase(RGResViewType inType);

        RGResViewType type;
    };

    template <typename RHIResourceView>
    struct RHIResourceViewTraits {};

    template <typename RHIResourceView>
    class RGResourceView : public RGResourceViewBase {
    public:
        using ResourceRef = typename RHIResourceViewTraits<RHIResourceView>::ResourceRefType;
        using Desc = typename RHIResourceViewTraits<RHIResourceView>::DescType;

        ~RGResourceView() override;

        const Desc& GetDesc() const;
        ResourceRef GetResource() const;
        RHIResourceView* GetRHI() const;
        RGResourceRef GetResourceBase() override;

    private:
        friend class RGBuilder;

        RGResourceView(ResourceRef inResource, Desc inDesc);

        ResourceRef resource;
        Desc desc;
        RHIResourceView* rhiHandle;
    };

    using RGBufferView = RGResourceView<RHI::BufferView>;
    using RGTextureView = RGResourceView<RHI::TextureView>;
    using RGResourceViewRef = RGResourceViewBase*;
    using RGBufferViewRef = RGBufferView*;
    using RGTextureViewRef = RGTextureView*;

    struct RGRasterPassDesc {
        std::vector<RGTextureViewRef> colorAttachments;
        RGTextureViewRef depthStencilAttachment;
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
        RHI::BindGroupLayout* layout;
        std::unordered_map<std::string, RGBindItemDesc> items;

        static RGBindGroupDesc Create(RHI::BindGroupLayout* inLayout);
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

        RGBindGroupDesc desc;
        RHI::BindGroup* rhiHandle;
    };

    using RGBindGroupRef = RGBindGroup*;

    class RGPass {
    public:
        virtual ~RGPass();

    protected:
        friend class RGBuilder;

        RGPass(std::string inName, RGPassType inType);

        RGPassType Type() const;
        virtual void Compile() = 0;

        std::string name;
        RGPassType type;
        std::unordered_set<RGResourceRef> reads;
        std::unordered_set<RGResourceRef> writes;
    };

    using RGCopyPassExecuteFunc = std::function<void(RHI::CommandEncoder&)>;
    using RGComputePassExecuteFunc = std::function<void(RHI::ComputePassCommandEncoder&)>;
    using RGRasterPassExecuteFunc = std::function<void(RHI::GraphicsPassCommandEncoder&)>;

    class RGCopyPass : public RGPass {
    public:
        ~RGCopyPass() override;

    private:
        friend class RGBuilder;

        RGCopyPass(std::string inName, RGCopyPassDesc inPassDesc, RGCopyPassExecuteFunc inFunc);
        void Compile() override;

        RGCopyPassDesc passDesc;
        RGCopyPassExecuteFunc func;
    };

    class RGComputePass : public RGPass {
    public:
        ~RGComputePass() override;

    private:
        friend class RGBuilder;

        RGComputePass(std::string inName, std::vector<RGBindGroupRef> inBindGroups, RGComputePassExecuteFunc inFunc, bool inAsyncCompute = false);
        void Compile() override;

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
        void Compile() override;

        RGRasterPassDesc passDesc;
        RGRasterPassExecuteFunc func;
        std::vector<RGBindGroupRef> bindGroups;
    };

    template <typename R>
    struct RGResOrViewTraits {};

    class RGBuilder {
    public:
        NonCopyable(RGBuilder);
        explicit RGBuilder(RHI::Device& inDevice);
        ~RGBuilder();

        template <typename ResOrView, typename... Args>
        requires std::derived_from<ResOrView, RGResourceBase> || std::derived_from<ResOrView, RGResourceViewBase>
        typename RGResOrViewTraits<ResOrView>::RefType Create(Args&&... args)
        {
            typename RGResOrViewTraits<ResOrView>::RefType ref = new ResOrView(std::forward<Args>(args)...);
            if constexpr (std::derived_from<ResOrView, RGResourceBase>) {
                resources.emplace_back(Common::UniqueRef<RGResourceBase>(ref));
                return resources.back().Get();
            } else {
                views.emplace_back(Common::UniqueRef<RGResourceViewBase>(ref));
                return views.back().Get();
            }
        }

        template <typename Res>
        requires std::derived_from<Res, RGResourceBase>
        typename RGResOrViewTraits<Res>::RefType Import(typename RGResOrViewTraits<Res>::RHIType* rhiHandle)
        {
            typename RGResOrViewTraits<Res>::RefType ref = new Res(rhiHandle->GetCreateInfo());
            ref->rhiHandle = rhiHandle;
            resources.emplace_back(Common::UniqueRef<RGResourceBase>(ref));
            return resources.back().Get();
        }

        RGBindGroupRef AllocateBindGroup(const RGBindGroupDesc& inDesc)
        {
            bindGroups.emplace_back(Common::UniqueRef<RGBindGroup>(new RGBindGroup(inDesc)));
            return bindGroups.back().Get();
        }

        template <typename P, typename... Args>
        void Add(Args&&... args)
        {
            passes.emplace_back(Common::UniqueRef<RGPass>(new P(std::forward<Args>(args)...)));
        }

        void Execute(RHI::Fence* mainFence, RHI::Fence* asyncComputeFence = nullptr, RHI::Fence* asyncCopyFence = nullptr);

    private:
        void Compile();
        void CompilePasses();
        void DevirtualizeResources();

        RHI::Device& device;
        std::vector<Common::UniqueRef<RGResourceBase>> resources;
        std::vector<Common::UniqueRef<RGResourceViewBase>> views;
        std::vector<Common::UniqueRef<RGBindGroup>> bindGroups;
        std::vector<Common::UniqueRef<RGPass>> passes;
    };
}

namespace Rendering {
    template <>
    struct RHIResourceTraits<RHI::Buffer> {
        using DescType = RGBufferDesc;
        static constexpr RGResType type = RGResType::buffer;
    };

    template <>
    struct RHIResourceTraits<RHI::Texture> {
        using DescType = RGTextureDesc;
        static constexpr RGResType type = RGResType::texture;
    };

    template <>
    struct RHIResourceViewTraits<RHI::BufferView> {
        using ResourceRefType = RGBufferRef;
        using DescType = RGBufferViewDesc;
        static constexpr RGResViewType type = RGResViewType::bufferView;
    };

    template <>
    struct RHIResourceViewTraits<RHI::TextureView> {
        using ResourceRefType = RGTextureRef;
        using DescType = RGTextureViewDesc;
        static constexpr RGResViewType type = RGResViewType::textureView;
    };

    template <>
    struct RGResOrViewTraits<RGBuffer> {
        using RefType = RGBufferRef;
        using RHIType = RHI::Buffer;
        using DescType = RGBufferDesc;
    };

    template <>
    struct RGResOrViewTraits<RGTexture> {
        using RefType = RGTextureRef;
        using RHIType = RHI::Texture;
        using DescType = RGTextureDesc;
    };

    template <>
    struct RGResOrViewTraits<RGBufferView> {
        using RefType = RGBufferViewRef;
        using RHIType = RHI::BufferView;
        using DescType = RGBufferViewDesc;
    };

    template <>
    struct RGResOrViewTraits<RGTextureView> {
        using RefType = RGTextureViewRef;
        using RHIType = RHI::TextureView;
        using DescType = RGTextureViewDesc;
    };

    template <typename RHIResource>
    RGResource<RHIResource>::RGResource(Desc inDesc)
        : desc(std::move(inDesc))
    {
    }

    template <typename RHIResource>
    RGResource<RHIResource>::~RGResource() = default;

    template <typename RHIResource>
    const typename RGResource<RHIResource>::Desc& RGResource<RHIResource>::GetDesc() const
    {
        return desc;
    }

    template <typename RHIResource>
    RHIResource* RGResource<RHIResource>::GetRHI() const
    {
        return rhiHandle;
    }

    template <typename RHIResourceView>
    RGResourceView<RHIResourceView>::RGResourceView(ResourceRef inResource, RGResourceView::Desc inDesc)
        : resource(inResource)
        , desc(std::move(inDesc))
    {
    }

    template <typename RHIResourceView>
    RGResourceView<RHIResourceView>::~RGResourceView() = default;

    template <typename RHIResourceView>
    const typename RGResourceView<RHIResourceView>::Desc& RGResourceView<RHIResourceView>::GetDesc() const
    {
        return desc;
    }

    template <typename RHIResourceView>
    RGResourceView<RHIResourceView>::ResourceRef RGResourceView<RHIResourceView>::GetResource() const
    {
        return resource;
    }

    template <typename RHIResourceView>
    RHIResourceView* RGResourceView<RHIResourceView>::GetRHI() const
    {
        return rhiHandle;
    }

    template <typename RHIResourceView>
    RGResourceRef RGResourceView<RHIResourceView>::GetResourceBase()
    {
        return resource;
    }
}
