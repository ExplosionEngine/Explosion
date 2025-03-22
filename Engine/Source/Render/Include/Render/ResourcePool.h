//
// Created by johnk on 2023/12/11.
//

#pragma once

#include <unordered_map>

#include <Common/Memory.h>
#include <Common/Container.h>
#include <Core/Thread.h>
#include <RHI/RHI.h>

namespace Render::Internal {
    constexpr uint64_t pooledResourceReleaseFrameLatency = 2;
}

namespace Render {
    template <typename RHIRes>
    struct RHIResTraits {};

    template <typename RHIRes>
    class PooledResource {
    public:
        using DescType = typename RHIResTraits<RHIRes>::DescType;

        explicit PooledResource(Common::UniquePtr<RHIRes>&& inRhiHandle, DescType inDesc);
        ~PooledResource();

        RHIRes* GetRHI() const;
        const DescType& GetDesc() const;
        uint64_t LastUsedFrame() const;
        void MarkUsedThisFrame();

    private:
        Common::UniquePtr<RHIRes> rhiHandle;
        DescType desc;
        uint64_t lastUsedFrame;
    };

    using PooledBuffer = PooledResource<RHI::Buffer>;
    using PooledTexture = PooledResource<RHI::Texture>;
    using PooledBufferDesc = RHI::BufferCreateInfo;
    using PooledTextureDesc = RHI::TextureCreateInfo;
    using PooledBufferRef = Common::SharedPtr<PooledBuffer>;
    using PooledTextureRef = Common::SharedPtr<PooledTexture>;

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
        void Forfeit();
        void Invalidate();

    private:
        explicit ResourcePool(RHI::Device& inDevice);

        RHI::Device& device;
        std::vector<ResRefType> pooledResources;
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
    PooledResource<RHIResource>::PooledResource(Common::UniquePtr<RHIResource>&& inRhiHandle, DescType inDesc)
        : rhiHandle(std::move(inRhiHandle))
        , desc(std::move(inDesc))
        , lastUsedFrame(Core::ThreadContext::FrameNumber())
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

    template <typename RHIRes>
    uint64_t PooledResource<RHIRes>::LastUsedFrame() const
    {
        return lastUsedFrame;
    }

    template <typename RHIRes>
    void PooledResource<RHIRes>::MarkUsedThisFrame()
    {
        lastUsedFrame = Core::ThreadContext::FrameNumber();
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
        static std::unordered_map<RHI::Device*, Common::UniquePtr<ResourcePool>> deviceMap;
        if (!deviceMap.contains(&device)) {
            deviceMap.emplace(std::make_pair(&device, Common::UniquePtr<ResourcePool>(new ResourcePool(device))));
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
        for (auto& pooledResource : pooledResources) {
            if (pooledResource.RefCount() == 1 && desc == pooledResource->GetDesc()) {
                pooledResource->MarkUsedThisFrame();
                return pooledResource;
            }
        }
        auto result = PooledResTraits<PooledResource>::CreateResource(device, desc);
        pooledResources.emplace_back(result);
        return result;
    }

    template <typename PooledRes>
    size_t ResourcePool<PooledRes>::Size() const
    {
        return pooledResources.size();
    }

    template <typename PooledRes>
    void ResourcePool<PooledRes>::Forfeit()
    {
        const auto currentFrame = Core::ThreadContext::FrameNumber();

        for (auto i = 0; i < pooledResources.size();) {
            bool needRelease = false;
            auto& pooledResource = pooledResources[i];

            if (pooledResource.RefCount() <= 1) {
                needRelease = currentFrame - pooledResource->LastUsedFrame() > Internal::pooledResourceReleaseFrameLatency;
            } else {
                pooledResource->MarkUsedThisFrame();
            }

            if (needRelease) { // NOLINT
                pooledResources.erase(pooledResources.begin() + i);
            } else {
                i++;
            }
        }
    }

    template <typename PooledRes>
    void ResourcePool<PooledRes>::Invalidate()
    {
        for (const auto& pooledResource : pooledResources) {
            Assert(pooledResource.RefCount() == 1);
        }
        pooledResources.clear();
    }
} // namespace Render
