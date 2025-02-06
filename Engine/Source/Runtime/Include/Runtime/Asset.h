//
// Created by johnk on 2023/10/10.
//

#pragma once

#include <string>
#include <functional>
#include <unordered_map>
#include <utility>

#include <Common/Memory.h>
#include <Common/Serialization.h>
#include <Common/Concurrent.h>
#include <Common/Concepts.h>
#include <Core/Uri.h>
#include <Mirror/Meta.h>
#include <Mirror/Mirror.h>
#include <Runtime/Api.h>

namespace Runtime {
    struct RUNTIME_API EClass() Asset {
        EClassBody(Asset)

        Asset();
        explicit Asset(Core::Uri inUri);

        EProperty() Core::Uri uri;
    };

    template <Common::DerivedFrom<Asset> A>
    class AssetRef {
    public:
        template <Common::DerivedFrom<Asset> A2> AssetRef(Common::SharedRef<A2>& inRef); // NOLINT
        template <Common::DerivedFrom<Asset> A2> AssetRef(Common::SharedRef<A2>&& inRef) noexcept; // NOLINT
        AssetRef(A* pointer); // NOLINT
        AssetRef(AssetRef& other); // NOLINT
        AssetRef(AssetRef&& other) noexcept; // NOLINT
        AssetRef();
        ~AssetRef();

        template <Common::DerivedFrom<Asset> A2> AssetRef& operator=(Common::SharedRef<A2>& inRef);
        template <Common::DerivedFrom<Asset> A2> AssetRef& operator=(Common::SharedRef<A2>&& inRef) noexcept;
        AssetRef& operator=(A* pointer);
        AssetRef& operator=(AssetRef& other); // NOLINT
        AssetRef& operator=(AssetRef&& other) noexcept;

        const Core::Uri& Uri() const;
        A* operator->() const noexcept;
        A& operator*() const noexcept;
        bool operator==(nullptr_t) const noexcept;
        bool operator!=(nullptr_t) const noexcept;
        A* Get() const;
        void Reset(A* pointer = nullptr);
        auto RefCount() const;
        Common::SharedRef<A>& GetSharedRef();
        template <Common::DerivedFrom<Asset> A2> AssetRef<A2> StaticCast();
        template <Common::DerivedFrom<Asset> A2> AssetRef<A2> DynamicCast();
        template <Common::DerivedFrom<Asset> A2> AssetRef<A2> ReinterpretCast();

    private:
        Common::SharedRef<A> ref;
    };

    template <Common::DerivedFrom<Asset> A>
    class WeakAssetRef {
    public:
        template <Common::DerivedFrom<Asset> A2> WeakAssetRef(AssetRef<A2>& inRef); // NOLINT
        WeakAssetRef(WeakAssetRef& other); // NOLINT
        WeakAssetRef(WeakAssetRef&& other) noexcept; // NOLINT

        template <Common::DerivedFrom<Asset> A2> WeakAssetRef& operator=(AssetRef<A2>& inRef);
        WeakAssetRef& operator=(WeakAssetRef& other); // NOLINT
        WeakAssetRef& operator=(WeakAssetRef&& other) noexcept;
        void Reset();
        bool Expired() const;
        AssetRef<A> Lock() const;

    private:
        Common::WeakRef<A> ref;
    };

    template <Common::DerivedFrom<Asset> A>
    class SoftAssetRef {
    public:
        SoftAssetRef();
        explicit SoftAssetRef(Core::Uri inUri);
        explicit SoftAssetRef(AssetRef<A>& inAsset);
        SoftAssetRef(const SoftAssetRef<A>& other);
        SoftAssetRef(SoftAssetRef<A>&& other) noexcept;
        ~SoftAssetRef();

        SoftAssetRef& operator=(Core::Uri inUri);
        SoftAssetRef& operator=(AssetRef<A>& inAsset);
        SoftAssetRef& operator=(const SoftAssetRef<A>& other);
        SoftAssetRef& operator=(SoftAssetRef<A>&& other) noexcept;

        bool Empty() const;
        bool Loaded() const;
        AssetRef<A> Get() const;
        void Reset();
        const Core::Uri& Uri() const;

    private:
        Core::Uri uri;
        AssetRef<A> asset;
    };

    template <Common::DerivedFrom<Asset> A> using OnAssetLoaded = std::function<void(AssetRef<A>)>;
    template <Common::DerivedFrom<Asset> A> using OnSoftAssetLoaded = std::function<void()>;

    class RUNTIME_API AssetManager {
    public:
        static AssetManager& Get();
        ~AssetManager();

        template <Common::DerivedFrom<Asset> A> AssetRef<A> SyncLoad(const Core::Uri& uri);
        template <Common::DerivedFrom<Asset> A> void SyncLoadSoft(SoftAssetRef<A>& softAssetRef);
        template <Common::DerivedFrom<Asset> A> void AsyncLoad(const Core::Uri& uri, const OnAssetLoaded<A>& onAssetLoaded);
        template <Common::DerivedFrom<Asset> A> void AsyncLoadSoft(SoftAssetRef<A>& softAssetRef, const OnSoftAssetLoaded<A>& onSoftAssetLoaded);
        template <Common::DerivedFrom<Asset> A> void Save(const AssetRef<A>& assetRef);
        template <Common::DerivedFrom<Asset> A> void SaveSoft(const SoftAssetRef<A>& softAssetRef);

    private:
        template <Common::DerivedFrom<Asset> A> AssetRef<A> LoadInternal(const Core::Uri& uri);

        AssetManager();

        std::mutex mutex;
        std::unordered_map<Core::Uri, WeakAssetRef<Asset>> weakAssetRefs;
        Common::ThreadPool threadPool;
    };
}

namespace Common {
    template <DerivedFrom<Runtime::Asset> A>
    struct Serializer<Runtime::AssetRef<A>> {
        static constexpr size_t typeId = Common::HashUtils::StrCrc32("Runtime::AssetRef");

        static size_t Serialize(BinarySerializeStream& stream, const Runtime::AssetRef<A>& value)
        {
            return Serializer<Core::Uri>::Serialize(stream, value);
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, Runtime::AssetRef<A>& value)
        {
            Core::Uri uri;
            const auto deserialized = Serializer<Core::Uri>::Deserialize(stream, uri);
            value = Runtime::AssetManager::Get().SyncLoad<A>(uri, uri);
            return deserialized;
        }
    };

    template <DerivedFrom<Runtime::Asset> A>
    struct Serializer<Runtime::SoftAssetRef<A>> {
        static constexpr size_t typeId = Common::HashUtils::StrCrc32("Runtime::SoftAssetRef");

        static size_t Serialize(BinarySerializeStream& stream, const Runtime::SoftAssetRef<A>& value)
        {
            return Serializer<Core::Uri>::Serialize(stream, value);
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, Runtime::SoftAssetRef<A>& value)
        {
            Core::Uri uri;
            const auto deserialized = Serializer<Core::Uri>::Deserialize(stream, uri);
            value = uri;
            return deserialized;
        }
    };
}

namespace Runtime {
    template <Common::DerivedFrom<Asset> A>
    template <Common::DerivedFrom<Asset> A2>
    AssetRef<A>::AssetRef(Common::SharedRef<A2>& inRef)
        : ref(inRef)
    {
    }

    template <Common::DerivedFrom<Asset> A>
    template <Common::DerivedFrom<Asset> A2>
    AssetRef<A>::AssetRef(Common::SharedRef<A2>&& inRef) noexcept
        : ref(std::move(inRef))
    {
    }

    template <Common::DerivedFrom<Asset> A>
    AssetRef<A>::AssetRef(A* pointer)
        : ref(pointer)
    {
    }

    template <Common::DerivedFrom<Asset> A>
    AssetRef<A>::AssetRef(AssetRef& other)
        : ref(other.ref)
    {
    }

    template <Common::DerivedFrom<Asset> A>
    AssetRef<A>::AssetRef(AssetRef&& other) noexcept
        : ref(std::move(other.ref))
    {
    }

    template <Common::DerivedFrom<Asset> A>
    AssetRef<A>::AssetRef() = default;

    template <Common::DerivedFrom<Asset> A>
    AssetRef<A>::~AssetRef() = default;

    template <Common::DerivedFrom<Asset> A>
    template <Common::DerivedFrom<Asset> A2>
    AssetRef<A>& AssetRef<A>::operator=(Common::SharedRef<A2>& inRef)
    {
        ref = inRef;
        return *this;
    }

    template <Common::DerivedFrom<Asset> A>
    template <Common::DerivedFrom<Asset> A2>
    AssetRef<A>& AssetRef<A>::operator=(Common::SharedRef<A2>&& inRef) noexcept
    {
        ref = inRef;
        return *this;
    }

    template <Common::DerivedFrom<Asset> A>
    AssetRef<A>& AssetRef<A>::operator=(A* pointer)
    {
        ref = pointer;
        return *this;
    }

    template <Common::DerivedFrom<Asset> A>
    AssetRef<A>& AssetRef<A>::operator=(AssetRef& other)
    {
        ref = other.ref;
        return *this;
    }

    template <Common::DerivedFrom<Asset> A>
    AssetRef<A>& AssetRef<A>::operator=(AssetRef&& other) noexcept
    {
        ref = std::move(other.ref);
        return *this;
    }

    template <Common::DerivedFrom<Asset> A>
    const Core::Uri& AssetRef<A>::Uri() const
    {
        Assert(ref != nullptr);
        return ref->uri;
    }

    template <Common::DerivedFrom<Asset> A>
    A* AssetRef<A>::operator->() const noexcept
    {
        return ref.operator->();
    }

    template <Common::DerivedFrom<Asset> A>
    A& AssetRef<A>::operator*() const noexcept
    {
        return ref.operator*();
    }

    template <Common::DerivedFrom<Asset> A>
    bool AssetRef<A>::operator==(nullptr_t) const noexcept
    {
        return ref == nullptr;
    }

    template <Common::DerivedFrom<Asset> A>
    bool AssetRef<A>::operator!=(nullptr_t) const noexcept
    {
        return ref != nullptr;
    }

    template <Common::DerivedFrom<Asset> A>
    A* AssetRef<A>::Get() const
    {
        return ref.Get();
    }

    template <Common::DerivedFrom<Asset> A>
    void AssetRef<A>::Reset(A* pointer)
    {
        ref.Reset(pointer);
    }

    template <Common::DerivedFrom<Asset> A>
    auto AssetRef<A>::RefCount() const
    {
        return ref.RefCount();
    }

    template <Common::DerivedFrom<Asset> A>
    Common::SharedRef<A>& AssetRef<A>::GetSharedRef()
    {
        return ref;
    }

    template <Common::DerivedFrom<Asset> A>
    template <Common::DerivedFrom<Asset> A2>
    AssetRef<A2> AssetRef<A>::StaticCast()
    {
        return ref.template StaticCast<A2>();
    }

    template <Common::DerivedFrom<Asset> A>
    template <Common::DerivedFrom<Asset> A2>
    AssetRef<A2> AssetRef<A>::DynamicCast()
    {
        return ref.template DynamicCast<A2>();
    }

    template <Common::DerivedFrom<Asset> A>
    template <Common::DerivedFrom<Asset> A2>
    AssetRef<A2> AssetRef<A>::ReinterpretCast()
    {
        return ref.template ReinterpretCast<A2>();
    }

    template <Common::DerivedFrom<Asset> A>
    template <Common::DerivedFrom<Asset> A2>
    WeakAssetRef<A>::WeakAssetRef(AssetRef<A2>& inRef)
        : ref(inRef.GetSharedRef())
    {
    }

    template <Common::DerivedFrom<Asset> A>
    WeakAssetRef<A>::WeakAssetRef(WeakAssetRef& other)
        : ref(other.ref)
    {
    }

    template <Common::DerivedFrom<Asset> A>
    WeakAssetRef<A>::WeakAssetRef(WeakAssetRef&& other) noexcept
        : ref(std::move(other.ref))
    {
    }

    template <Common::DerivedFrom<Asset> A>
    template <Common::DerivedFrom<Asset> A2>
    WeakAssetRef<A>& WeakAssetRef<A>::operator=(AssetRef<A2>& inRef)
    {
        ref = inRef.GetSharedRef();
        return *this;
    }

    template <Common::DerivedFrom<Asset> A>
    WeakAssetRef<A>& WeakAssetRef<A>::operator=(WeakAssetRef& other)
    {
        ref = other.ref;
        return *this;
    }

    template <Common::DerivedFrom<Asset> A>
    WeakAssetRef<A>& WeakAssetRef<A>::operator=(WeakAssetRef&& other) noexcept
    {
        ref = std::move(other.ref);
        return *this;
    }

    template <Common::DerivedFrom<Asset> A>
    void WeakAssetRef<A>::Reset()
    {
        ref.Reset();
    }

    template <Common::DerivedFrom<Asset> A>
    bool WeakAssetRef<A>::Expired() const
    {
        return ref.Expired();
    }

    template <Common::DerivedFrom<Asset> A>
    AssetRef<A> WeakAssetRef<A>::Lock() const
    {
        return ref.Lock();
    }

    template <Common::DerivedFrom<Asset> A>
    SoftAssetRef<A>::SoftAssetRef()
        : uri()
        , asset()
    {
    }

    template <Common::DerivedFrom<Asset> A>
    SoftAssetRef<A>::SoftAssetRef(Core::Uri inUri)
        : uri(std::move(inUri))
        , asset()
    {
    }

    template <Common::DerivedFrom<Asset> A>
    SoftAssetRef<A>::SoftAssetRef(AssetRef<A>& inAsset)
        : uri(inAsset.Uri())
        , asset(inAsset)
    {
    }

    template <Common::DerivedFrom<Asset> A>
    SoftAssetRef<A>::SoftAssetRef(const SoftAssetRef<A>& other)
        : uri(other.uri)
        , asset(other.asset)
    {
    }

    template <Common::DerivedFrom<Asset> A>
    SoftAssetRef<A>::SoftAssetRef(SoftAssetRef<A>&& other) noexcept
        : uri(std::move(other.uri))
        , asset(std::move(other.asset))
    {
    }

    template <Common::DerivedFrom<Asset> A>
    SoftAssetRef<A>::~SoftAssetRef() = default;

    template <Common::DerivedFrom<Asset> A>
    SoftAssetRef<A>& SoftAssetRef<A>::operator=(Core::Uri inUri)
    {
        uri = std::move(inUri);
        asset = nullptr;
        return *this;
    }

    template <Common::DerivedFrom<Asset> A>
    SoftAssetRef<A>& SoftAssetRef<A>::operator=(AssetRef<A>& inAsset)
    {
        uri = inAsset.Uri();
        asset = inAsset;
        return *this;
    }

    template <Common::DerivedFrom<Asset> A>
    SoftAssetRef<A>& SoftAssetRef<A>::operator=(const SoftAssetRef<A>& other)
    {
        uri = other.uri;
        asset = other.asset;
        return *this;
    }

    template <Common::DerivedFrom<Asset> A>
    SoftAssetRef<A>& SoftAssetRef<A>::operator=(SoftAssetRef<A>&& other) noexcept
    {
        uri = std::move(other.uri);
        asset = std::move(other.asset);
        return *this;
    }

    template <Common::DerivedFrom<Asset> A>
    bool SoftAssetRef<A>::Empty() const
    {
        return uri.Empty();
    }

    template <Common::DerivedFrom<Asset> A>
    bool SoftAssetRef<A>::Loaded() const
    {
        return asset != nullptr;
    }

    template <Common::DerivedFrom<Asset> A>
    AssetRef<A> SoftAssetRef<A>::Get() const
    {
        return asset;
    }

    template <Common::DerivedFrom<Asset> A>
    void SoftAssetRef<A>::Reset()
    {
        asset = nullptr;
    }

    template <Common::DerivedFrom<Asset> A>
    const Core::Uri& SoftAssetRef<A>::Uri() const
    {
        return uri;
    }

    template <Common::DerivedFrom<Asset> A>
    AssetRef<A> AssetManager::SyncLoad(const Core::Uri& uri)
    {
        auto iter = weakAssetRefs.find(uri);
        if (iter != weakAssetRefs.end() && !iter->second.Expired()) {
            return iter->second.Lock().StaticCast<A>();
        }

        AssetRef<A> result = LoadInternal<A>(uri);
        AssetRef<Asset> tempRef = result.template StaticCast<Asset>();
        if (iter == weakAssetRefs.end()) {
            weakAssetRefs.emplace(std::make_pair(uri, WeakAssetRef<Asset>(tempRef)));
        } else {
            iter->second = tempRef;
        }
        return result;
    }

    template <Common::DerivedFrom<Asset> A>
    void AssetManager::SyncLoadSoft(SoftAssetRef<A>& softAssetRef)
    {
        softAssetRef = SyncLoad<A>(softAssetRef.Uri());
    }

    template <Common::DerivedFrom<Asset> A>
    void AssetManager::AsyncLoad(const Core::Uri& uri, const OnAssetLoaded<A>& onAssetLoaded)
    {
        threadPool.EmplaceTask([=]() -> void {
            AssetRef<A> result = nullptr;
            {
                std::unique_lock<std::mutex> lock(mutex);
                auto iter = weakAssetRefs.find(uri);
                if (iter != weakAssetRefs.end() && !iter->second.Expired()) {
                    result = iter->second.Lock().StaticCast<A>();
                }
            }

            if (result == nullptr) {
                result = LoadInternal<A>(uri);
            }

            AssetRef<Asset> tempRef = result.template StaticCast<Asset>();
            {
                std::unique_lock<std::mutex> lock(mutex);
                auto iter = weakAssetRefs.find(uri);
                if (iter == weakAssetRefs.end()) {
                    weakAssetRefs.emplace(std::make_pair(uri, WeakAssetRef<Asset>(tempRef)));
                } else {
                    iter->second = tempRef;
                }
            }

            onAssetLoaded(result);
        });
    }

    template <Common::DerivedFrom<Asset> A>
    void AssetManager::AsyncLoadSoft(SoftAssetRef<A>& softAssetRef, const OnSoftAssetLoaded<A>& onSoftAssetLoaded)
    {
        threadPool.EmplaceTask([this, softAssetRef, onSoftAssetLoaded]() -> void {
            AsyncLoad(softAssetRef.Uri(), [&](AssetRef<A>& ref) -> void {
                softAssetRef = ref;
                onSoftAssetLoaded();
            });
        });
    }

    template <Common::DerivedFrom<Asset> A>
    void AssetManager::Save(const AssetRef<A>& assetRef)
    {
        if (assetRef == nullptr) {
            return;
        }

        Core::AssetUriParser parser(assetRef.Uri());
        auto pathString = parser.AbsoluteFilePath().String();
        Common::BinaryFileSerializeStream stream(pathString);

        Mirror::Any ref = std::ref(*assetRef.Get());
        ref.Serialize(stream);
    }

    template <Common::DerivedFrom<Asset> A>
    void AssetManager::SaveSoft(const SoftAssetRef<A>& softAssetRef)
    {
        Save(softAssetRef.Uri());
    }

    template <Common::DerivedFrom<Asset> A>
    AssetRef<A> AssetManager::LoadInternal(const Core::Uri& uri)
    {
        Core::AssetUriParser parser(uri);
        auto pathString = parser.AbsoluteFilePath().String();
        Common::BinaryFileDeserializeStream stream(pathString);

        AssetRef<A> result = Common::SharedRef<A>(new A());
        Mirror::Any ref = std::ref(*result.Get());
        ref.Deserialize(stream);

        // reset uri is useful for moved asset
        result->uri = uri;
        return result;
    }
}
