//
// Created by johnk on 2023/12/11.
//

#pragma once

#include <unordered_map>

#include <Common/Memory.h>
#include <Common/Debug.h>
#include <Common/Container.h>
#include <RHI/RHI.h>

namespace Rendering {
    template <typename RHIRes>
    struct RHIResTraits {};

    template <typename RHIRes>
    class PooledResource {
    public:
        using DescType = typename RHIResTraits<RHIRes>::DescType;

        explicit PooledResource(Common::UniqueRef<RHIRes>&& inRhiHandle, DescType inDesc);
        ~PooledResource();

        RHIRes* GetRHI() const;
        const DescType& GetDesc() const;

    private:
        Common::UniqueRef<RHIRes> rhiHandle;
        DescType desc;
    };

    using PooledBuffer = PooledResource<RHI::Buffer>;
    using PooledTexture = PooledResource<RHI::Texture>;
    using PooledBufferDesc = RHI::BufferCreateInfo;
    using PooledTextureDesc = RHI::TextureCreateInfo;
    using PooledBufferRef = Common::SharedRef<PooledBuffer>;
    using PooledTextureRef = Common::SharedRef<PooledTexture>;

    template <typename PooledRes>
    struct PooledResTraits {};

    template <typename PooledRes>
    class RGResourcePool {
    public:
        using ResRefType = typename PooledResTraits<PooledRes>::RefType;
        using DescType = typename PooledResTraits<PooledRes>::DescType;

        static RGResourcePool& Get(RHI::Device& device);
        ~RGResourcePool();

        ResRefType Allocate(const DescType& desc);

    private:
        explicit RGResourcePool(RHI::Device& inDevice);

        RHI::Device& device;
        std::vector<ResRefType> pooledResources;
    };

    using BufferPool = RGResourcePool<PooledBuffer>;
    using TexturePool = RGResourcePool<PooledTexture>;
}

namespace Rendering {
    template <>
    struct RHIResTraits<RHI::Buffer> {
        using DescType = RHI::BufferCreateInfo;
    };

    template <>
    struct RHIResTraits<RHI::Texture> {
        using DescType = RHI::TextureCreateInfo;
    };

    template <typename RHIResource>
    PooledResource<RHIResource>::PooledResource(Common::UniqueRef<RHIResource>&& inRhiHandle, DescType inDesc)
        : rhiHandle(std::move(inRhiHandle))
        , desc(std::move(inDesc))
    {
    }

    template <typename RHIResource>
    PooledResource<RHIResource>::~PooledResource() = default;

    template <typename RHIResource>
    RHIResource* PooledResource<RHIResource>::GetRHI() const
    {
        return rhiHandle;
    }

    template <typename RHIResource>
    const typename PooledResource<RHIResource>::DescType& PooledResource<RHIResource>::GetDesc() const
    {
        return desc;
    }

    template <>
    struct PooledResTraits<PooledBuffer> {
        using ResType = PooledBuffer;
        using RefType = PooledBufferRef;
        using DescType = PooledBuffer::DescType;

        static RefType CreateResource(RHI::Device& device, const DescType& desc)
        {
            return { new ResType(device.CreateBuffer(desc), desc) };
        }
    };

    template <>
    struct PooledResTraits<PooledTexture> {
        using ResType = PooledTexture;
        using RefType = PooledTextureRef;
        using DescType = PooledTexture::DescType;

        static RefType CreateResource(RHI::Device& device, const DescType& desc)
        {
            return { new ResType(device.CreateTexture(desc), desc) };
        }
    };

    template <typename PooledResource>
    RGResourcePool<PooledResource>& RGResourcePool<PooledResource>::Get(RHI::Device& device)
    {
        static std::unordered_map<RHI::Device*, Common::UniqueRef<RGResourcePool>> deviceMap;
        if (!deviceMap.contains(&device)) {
            deviceMap.emplace(std::make_pair(&device, Common::UniqueRef<RGResourcePool>(new RGResourcePool(device))));
        }
        return *deviceMap.at(&device);
    }

    template <typename PooledResource>
    RGResourcePool<PooledResource>::RGResourcePool(RHI::Device& inDevice)
        : device(inDevice)
    {
    }

    template <typename PooledResource>
    RGResourcePool<PooledResource>::~RGResourcePool() = default;

    template <typename PooledResource>
    typename RGResourcePool<PooledResource>::ResRefType RGResourcePool<PooledResource>::Allocate(const DescType& desc)
    {
        for (const auto& pooledResource : pooledResources) {
            if (pooledResource.RefCount() == 1 && desc == pooledResource->GetDesc()) {
                return pooledResource;
            }
        }
        auto result = PooledResTraits<PooledResource>::CreateResource(device, desc);
        pooledResources.push_back(result);
        return result;
    }
}
