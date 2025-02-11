//
// Created by johnk on 2023/12/11.
//

#pragma once

#include <unordered_map>

#include <Common/Memory.h>
#include <Common/Debug.h>
#include <Common/Container.h>
#include <RHI/RHI.h>

namespace Render {
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
    class ResourcePool {
    public:
        using ResRefType = typename PooledResTraits<PooledRes>::RefType;
        using DescType = typename PooledResTraits<PooledRes>::DescType;

        static ResourcePool& Get(RHI::Device& device);

        ResRefType Allocate(const DescType& desc);
        size_t Size() const;
        void Tick();

    private:
        explicit ResourcePool(RHI::Device& inDevice);

        RHI::Device& device;
        std::vector<std::pair<ResRefType, uint32_t>> pooledResourceAndAges;
    };

    using BufferPool = ResourcePool<PooledBuffer>;
    using TexturePool = ResourcePool<PooledTexture>;
}

namespace Render {
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
        return rhiHandle.Get();
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
    ResourcePool<PooledResource>& ResourcePool<PooledResource>::Get(RHI::Device& device)
    {
        static std::unordered_map<RHI::Device*, Common::UniqueRef<ResourcePool>> deviceMap;
        if (!deviceMap.contains(&device)) {
            deviceMap.emplace(std::make_pair(&device, Common::UniqueRef<ResourcePool>(new ResourcePool(device))));
        }
        return *deviceMap.at(&device);
    }

    template <typename PooledResource>
    ResourcePool<PooledResource>::ResourcePool(RHI::Device& inDevice)
        : device(inDevice)
    {
    }

    template <typename PooledResource>
    typename ResourcePool<PooledResource>::ResRefType ResourcePool<PooledResource>::Allocate(const DescType& desc)
    {
        for (auto& [pooledResource, age] : pooledResourceAndAges) {
            if (pooledResource.RefCount() == 1 && desc == pooledResource->GetDesc()) {
                age = 0;
                return pooledResource;
            }
        }
        auto result = PooledResTraits<PooledResource>::CreateResource(device, desc);
        pooledResourceAndAges.emplace_back(result, 0);
        return result;
    }

    template <typename PooledRes>
    size_t ResourcePool<PooledRes>::Size() const
    {
        return pooledResourceAndAges.size();
    }

    template <typename PooledRes>
    void ResourcePool<PooledRes>::Tick()
    {
        for (auto i = 0; i < pooledResourceAndAges.size();) {
            bool needRelease = false;
            auto& [pooledResource, age] = pooledResourceAndAges[i];

            if (pooledResource.RefCount() <= 1) {
                needRelease = ++age >= 2;
            } else {
                age = 0;
            }

            if (needRelease) { // NOLINT
                pooledResourceAndAges.erase(pooledResourceAndAges.begin() + i);
            } else {
                i++;
            }
        }
    }
}
