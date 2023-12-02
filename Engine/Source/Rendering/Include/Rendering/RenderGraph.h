//
// Created by johnk on 2023/11/28.
//

#pragma once

#include <unordered_map>

#include <Common/Memory.h>
#include <Common/Debug.h>
#include <RHI/RHI.h>

namespace Rendering {
    template <typename RHIResource>
    struct RHIResourceTraits {};

    template <typename RHIResource>
    class RGPooledResource {
    public:
        using RHIResourceDescType = typename RHIResourceTraits<RHIResource>::DescType;

        explicit RGPooledResource(RHIResource* inRhiHandle, RHIResourceDescType inDesc);
        ~RGPooledResource();

        RHIResource* GetRHI() const;
        const RHIResourceDescType& GetDesc() const;

    private:
        RHIResource* rhiHandle;
        RHIResourceDescType desc;
    };

    using RGPooledBuffer = RGPooledResource<RHI::Buffer>;
    using RGPooledTexture = RGPooledResource<RHI::Texture>;
    using RGPooledBufferRef = Common::SharedRef<RGPooledBuffer>;
    using RGPooledTextureRef = Common::SharedRef<RGPooledTexture>;

    template <typename PooledResource>
    struct RGPooledResourceTraits {};

    template <typename PooledResource>
    class RGResourcePool {
    public:
        using ResourceRefType = typename RGPooledResourceTraits<PooledResource>::RefType;
        using ResourceDescType = typename RGPooledResourceTraits<PooledResource>::DescType;

        static RGResourcePool& Get(RHI::Device& device);
        ~RGResourcePool();

        ResourceRefType Allocate(const ResourceDescType& desc);

    private:
        explicit RGResourcePool(RHI::Device& inDevice);

        RHI::Device& device;
        std::vector<ResourceRefType> pooledResources;
    };

    using RGBufferPool = RGResourcePool<RGPooledBuffer>;
    using RGTexturePool = RGResourcePool<RGPooledTexture>;
}

namespace Rendering {
    template <>
    struct RHIResourceTraits<RHI::Buffer> {
        using DescType = RHI::BufferCreateInfo;
    };

    template <>
    struct RHIResourceTraits<RHI::Texture> {
        using DescType = RHI::TextureCreateInfo;
    };

    template <typename RHIResource>
    RGPooledResource<RHIResource>::RGPooledResource(RHIResource* inRhiHandle, RHIResourceDescType inDesc)
        : rhiHandle(inRhiHandle)
        , desc(std::move(inDesc))
    {
    }

    template <typename RHIResource>
    RGPooledResource<RHIResource>::~RGPooledResource()
    {
        rhiHandle->Destroy();
    }

    template <typename RHIResource>
    RHIResource* RGPooledResource<RHIResource>::GetRHI() const
    {
        return rhiHandle;
    }

    template <typename RHIResource>
    const RGPooledResource<RHIResource>::RHIResourceDescType& RGPooledResource<RHIResource>::GetDesc() const
    {
        return desc;
    }

    template <>
    struct RGPooledResourceTraits<RGPooledBuffer> {
        using ResType = RGPooledBuffer;
        using RefType = RGPooledBufferRef;
        using DescType = typename RGPooledBuffer::RHIResourceDescType;

        static RefType CreateResource(RHI::Device& device, const DescType& desc)
        {
            return Common::MakeShared<ResType>(device.CreateBuffer(desc), desc);
        }
    };

    template <>
    struct RGPooledResourceTraits<RGPooledTexture> {
        using ResType = RGPooledTexture;
        using RefType = RGPooledTextureRef;
        using DescType = typename RGPooledTexture::RHIResourceDescType;

        static RefType CreateResource(RHI::Device& device, const DescType& desc)
        {
            return Common::MakeShared<ResType>(device.CreateTexture(desc), desc);
        }
    };

    template <typename PooledResource>
    RGResourcePool<PooledResource>& RGResourcePool<PooledResource>::Get(RHI::Device& device)
    {
        static std::unordered_map<RHI::Device*, Common::UniqueRef<RGResourcePool<PooledResource>>> deviceMap;
        if (!deviceMap.contains(&device)) {
            deviceMap.emplace(std::make_pair(&device, Common::MakeUnique<RGResourcePool<PooledResource>>>(device)));
        }
        return deviceMap.at(&device);
    }

    template <typename PooledResource>
    RGResourcePool<PooledResource>::RGResourcePool(RHI::Device& inDevice)
        : device(inDevice)
    {
    }

    template <typename PooledResource>
    RGResourcePool<PooledResource>::~RGResourcePool()
    {
        for (auto& pooledResource : pooledResources) {
            Assert(pooledResource->RefCount() == 1);
        }
        pooledResources.clear();
    }

    template <typename PooledResource>
    RGResourcePool<PooledResource>::ResourceRefType RGResourcePool<PooledResource>::Allocate(const RGResourcePool<PooledResource>::ResourceDescType& desc)
    {
        // TODO
    }
}
