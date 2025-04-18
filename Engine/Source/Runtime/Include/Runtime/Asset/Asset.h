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
#include <Runtime/Meta.h>
#include <Mirror/Mirror.h>
#include <Runtime/Api.h>

namespace Runtime {
    class RUNTIME_API EClass() Asset {
        EPolyClassBody(Asset)

    public:
        Asset();
        explicit Asset(Core::Uri inUri);
        virtual ~Asset();

        const Core::Uri& Uri() const;
        void SetUri(Core::Uri inUri);

        virtual void PostLoad();

    private:
        EProperty() Core::Uri uri;
    };

    template <Common::DerivedFrom<Asset> A>
    class AssetPtr {
    public:
        template <Common::DerivedFrom<Asset> A2> AssetPtr(const Common::SharedPtr<A2>& sharedPtr); // NOLINT
        template <Common::DerivedFrom<Asset> A2> AssetPtr(Common::SharedPtr<A2>&& sharedPtr) noexcept; // NOLINT
        AssetPtr(A* pointer); // NOLINT
        AssetPtr(const AssetPtr& other); // NOLINT
        AssetPtr(AssetPtr&& other) noexcept; // NOLINT
        AssetPtr();
        ~AssetPtr();

        template <Common::DerivedFrom<Asset> A2> AssetPtr& operator=(const Common::SharedPtr<A2>& sharedPtr);
        template <Common::DerivedFrom<Asset> A2> AssetPtr& operator=(Common::SharedPtr<A2>&& sharedPtr) noexcept;
        AssetPtr& operator=(A* pointer);
        AssetPtr& operator=(const AssetPtr& other); // NOLINT
        AssetPtr& operator=(AssetPtr&& other) noexcept;

        const Core::Uri& Uri() const;
        A* operator->() const noexcept;
        A& operator*() const noexcept;
        explicit operator bool() const;
        bool operator==(nullptr_t) const noexcept;
        bool operator!=(nullptr_t) const noexcept;
        bool Valid() const;
        A* Get() const;
        void Reset(A* pointer = nullptr);
        auto RefCount() const;
        Common::SharedPtr<A>& GetSharedRef();
        template <Common::DerivedFrom<Asset> A2> AssetPtr<A2> StaticCast();
        template <Common::DerivedFrom<Asset> A2> AssetPtr<A2> DynamicCast();
        template <Common::DerivedFrom<Asset> A2> AssetPtr<A2> ReinterpretCast();

    private:
        Common::SharedPtr<A> ptr;
    };

    template <Common::DerivedFrom<Asset> A>
    class WeakAssetPtr {
    public:
        template <Common::DerivedFrom<Asset> A2> WeakAssetPtr(AssetPtr<A2>& assetPtr); // NOLINT
        WeakAssetPtr(WeakAssetPtr& other); // NOLINT
        WeakAssetPtr(WeakAssetPtr&& other) noexcept; // NOLINT

        template <Common::DerivedFrom<Asset> A2> WeakAssetPtr& operator=(AssetPtr<A2>& assetPtr);
        WeakAssetPtr& operator=(WeakAssetPtr& other); // NOLINT
        WeakAssetPtr& operator=(WeakAssetPtr&& other) noexcept;
        void Reset();
        bool Expired() const;
        AssetPtr<A> Lock() const;

    private:
        Common::WeakPtr<A> ptr;
    };

    template <Common::DerivedFrom<Asset> A>
    class SoftAssetPtr {
    public:
        SoftAssetPtr();
        SoftAssetPtr(Core::Uri inUri, const Mirror::Class& inClass);
        explicit SoftAssetPtr(AssetPtr<A>& inAsset);
        SoftAssetPtr(const SoftAssetPtr<A>& other);
        SoftAssetPtr(SoftAssetPtr<A>&& other) noexcept;
        ~SoftAssetPtr();

        SoftAssetPtr& operator=(const std::pair<Core::Uri, const Mirror::Class&>& inUriAndClass);
        SoftAssetPtr& operator=(AssetPtr<A>& inAsset);
        SoftAssetPtr& operator=(const SoftAssetPtr<A>& other);
        SoftAssetPtr& operator=(SoftAssetPtr<A>&& other) noexcept;

        bool Empty() const;
        bool Loaded() const;
        AssetPtr<A> Get() const;
        void Reset();
        const Core::Uri& Uri() const;
        const Mirror::Class* Class() const;

    private:
        Core::Uri uri;
        const Mirror::Class* clazz;
        AssetPtr<A> asset;
    };

    template <Common::DerivedFrom<Asset> A> using OnAssetLoaded = std::function<void(AssetPtr<A>)>;
    template <Common::DerivedFrom<Asset> A> using OnSoftAssetLoaded = std::function<void()>;

    class RUNTIME_API AssetManager {
    public:
        static AssetManager& Get();
        ~AssetManager();

        template <Common::DerivedFrom<Asset> A> AssetPtr<A> SyncLoad(const Core::Uri& uri, const Mirror::Class& clazz);
        template <Common::DerivedFrom<Asset> A> void SyncLoadSoft(SoftAssetPtr<A>& softAssetRef, const Mirror::Class& clazz);
        template <Common::DerivedFrom<Asset> A> void AsyncLoad(const Core::Uri& uri, const Mirror::Class& clazz, const OnAssetLoaded<A>& onAssetLoaded);
        template <Common::DerivedFrom<Asset> A> void AsyncLoadSoft(SoftAssetPtr<A>& softAssetRef, const Mirror::Class& clazz, const OnSoftAssetLoaded<A>& onSoftAssetLoaded);
        template <Common::DerivedFrom<Asset> A> void Save(const AssetPtr<A>& assetRef);
        template <Common::DerivedFrom<Asset> A> void SaveSoft(const SoftAssetPtr<A>& softAssetRef);

    private:
        template <Common::DerivedFrom<Asset> A> AssetPtr<A> LoadInternal(const Core::Uri& uri, const Mirror::Class& clazz);

        AssetManager();

        std::mutex mutex;
        std::unordered_map<Core::Uri, WeakAssetPtr<Asset>> weakAssetRefs;
        Common::ThreadPool threadPool;
    };
}

namespace Common {
    template <DerivedFrom<Runtime::Asset> A>
    struct Serializer<Runtime::AssetPtr < A>> {
        static constexpr size_t typeId = Common::HashUtils::StrCrc32("Runtime::AssetRef");

        static size_t Serialize(BinarySerializeStream& stream, const Runtime::AssetPtr<A>& value)
        {
            size_t serialized = 0;
            serialized += Serializer<Core::Uri>::Serialize(stream, value.Uri());
            serialized += Serializer<const Mirror::Class*>::Serialize(stream, &value->GetClass());
            return serialized;
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, Runtime::AssetPtr<A>& value)
        {
            size_t deserialized = 0;

            Core::Uri uri;
            deserialized += Serializer<Core::Uri>::Deserialize(stream, uri);

            const Mirror::Class* clazz;
            deserialized += Serializer<const Mirror::Class*>::Deserialize(stream, clazz);

            value = Runtime::AssetManager::Get().SyncLoad<A>(uri, *clazz);
            return deserialized;
        }
    };

    template <DerivedFrom<Runtime::Asset> A>
    struct Serializer<Runtime::SoftAssetPtr<A>> {
        static constexpr size_t typeId = Common::HashUtils::StrCrc32("Runtime::SoftAssetRef");

        static size_t Serialize(BinarySerializeStream& stream, const Runtime::SoftAssetPtr<A>& value)
        {
            size_t serialized = 0;
            serialized += Serializer<Core::Uri>::Serialize(stream, value.Uri());
            serialized += Serializer<const Mirror::Class*>::Serialize(stream, &value->GetClass());
            return serialized;
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, Runtime::SoftAssetPtr<A>& value)
        {
            size_t deserialized = 0;

            Core::Uri uri;
            deserialized += Serializer<Core::Uri>::Deserialize(stream, uri);

            const Mirror::Class* clazz;
            deserialized += Serializer<const Mirror::Class*>::Deserialize(stream, clazz);

            value = { value, *clazz };
            return deserialized;
        }
    };
}

namespace Runtime {
    template <Common::DerivedFrom<Asset> A>
    template <Common::DerivedFrom<Asset> A2>
    AssetPtr<A>::AssetPtr(const Common::SharedPtr<A2>& sharedPtr)
        : ptr(sharedPtr)
    {
    }

    template <Common::DerivedFrom<Asset> A>
    template <Common::DerivedFrom<Asset> A2>
    AssetPtr<A>::AssetPtr(Common::SharedPtr<A2>&& sharedPtr) noexcept
        : ptr(std::move(sharedPtr))
    {
    }

    template <Common::DerivedFrom<Asset> A>
    AssetPtr<A>::AssetPtr(A* pointer)
        : ptr(pointer)
    {
    }

    template <Common::DerivedFrom<Asset> A>
    AssetPtr<A>::AssetPtr(const AssetPtr& other)
        : ptr(other.ptr)
    {
    }

    template <Common::DerivedFrom<Asset> A>
    AssetPtr<A>::AssetPtr(AssetPtr&& other) noexcept
        : ptr(std::move(other.ptr))
    {
    }

    template <Common::DerivedFrom<Asset> A>
    AssetPtr<A>::AssetPtr() = default;

    template <Common::DerivedFrom<Asset> A>
    AssetPtr<A>::~AssetPtr() = default;

    template <Common::DerivedFrom<Asset> A>
    template <Common::DerivedFrom<Asset> A2>
    AssetPtr<A>& AssetPtr<A>::operator=(const Common::SharedPtr<A2>& sharedPtr)
    {
        ptr = sharedPtr;
        return *this;
    }

    template <Common::DerivedFrom<Asset> A>
    template <Common::DerivedFrom<Asset> A2>
    AssetPtr<A>& AssetPtr<A>::operator=(Common::SharedPtr<A2>&& sharedPtr) noexcept
    {
        ptr = sharedPtr;
        return *this;
    }

    template <Common::DerivedFrom<Asset> A>
    AssetPtr<A>& AssetPtr<A>::operator=(A* pointer)
    {
        ptr = pointer;
        return *this;
    }

    template <Common::DerivedFrom<Asset> A>
    AssetPtr<A>& AssetPtr<A>::operator=(const AssetPtr& other)
    {
        ptr = other.ptr;
        return *this;
    }

    template <Common::DerivedFrom<Asset> A>
    AssetPtr<A>& AssetPtr<A>::operator=(AssetPtr&& other) noexcept
    {
        ptr = std::move(other.ptr);
        return *this;
    }

    template <Common::DerivedFrom<Asset> A>
    const Core::Uri& AssetPtr<A>::Uri() const
    {
        Assert(ptr != nullptr);
        return ptr->Uri();
    }

    template <Common::DerivedFrom<Asset> A>
    A* AssetPtr<A>::operator->() const noexcept
    {
        return ptr.operator->();
    }

    template <Common::DerivedFrom<Asset> A>
    A& AssetPtr<A>::operator*() const noexcept
    {
        return ptr.operator*();
    }

    template <Common::DerivedFrom<Asset> A>
    AssetPtr<A>::operator bool() const
    {
        return Valid();
    }

    template <Common::DerivedFrom<Asset> A>
    bool AssetPtr<A>::operator==(nullptr_t) const noexcept
    {
        return ptr == nullptr;
    }

    template <Common::DerivedFrom<Asset> A>
    bool AssetPtr<A>::operator!=(nullptr_t) const noexcept
    {
        return ptr != nullptr;
    }

    template <Common::DerivedFrom<Asset> A>
    bool AssetPtr<A>::Valid() const
    {
        return ptr != nullptr;
    }

    template <Common::DerivedFrom<Asset> A>
    A* AssetPtr<A>::Get() const
    {
        return ptr.Get();
    }

    template <Common::DerivedFrom<Asset> A>
    void AssetPtr<A>::Reset(A* pointer)
    {
        ptr.Reset(pointer);
    }

    template <Common::DerivedFrom<Asset> A>
    auto AssetPtr<A>::RefCount() const
    {
        return ptr.RefCount();
    }

    template <Common::DerivedFrom<Asset> A>
    Common::SharedPtr<A>& AssetPtr<A>::GetSharedRef()
    {
        return ptr;
    }

    template <Common::DerivedFrom<Asset> A>
    template <Common::DerivedFrom<Asset> A2>
    AssetPtr<A2> AssetPtr<A>::StaticCast()
    {
        return ptr.template StaticCast<A2>();
    }

    template <Common::DerivedFrom<Asset> A>
    template <Common::DerivedFrom<Asset> A2>
    AssetPtr<A2> AssetPtr<A>::DynamicCast()
    {
        return ptr.template DynamicCast<A2>();
    }

    template <Common::DerivedFrom<Asset> A>
    template <Common::DerivedFrom<Asset> A2>
    AssetPtr<A2> AssetPtr<A>::ReinterpretCast()
    {
        return ptr.template ReinterpretCast<A2>();
    }

    template <Common::DerivedFrom<Asset> A>
    template <Common::DerivedFrom<Asset> A2>
    WeakAssetPtr<A>::WeakAssetPtr(AssetPtr<A2>& assetPtr)
        : ptr(assetPtr.GetSharedRef())
    {
    }

    template <Common::DerivedFrom<Asset> A>
    WeakAssetPtr<A>::WeakAssetPtr(WeakAssetPtr& other)
        : ptr(other.ptr)
    {
    }

    template <Common::DerivedFrom<Asset> A>
    WeakAssetPtr<A>::WeakAssetPtr(WeakAssetPtr&& other) noexcept
        : ptr(std::move(other.ptr))
    {
    }

    template <Common::DerivedFrom<Asset> A>
    template <Common::DerivedFrom<Asset> A2>
    WeakAssetPtr<A>& WeakAssetPtr<A>::operator=(AssetPtr<A2>& assetPtr)
    {
        ptr = assetPtr.GetSharedRef();
        return *this;
    }

    template <Common::DerivedFrom<Asset> A>
    WeakAssetPtr<A>& WeakAssetPtr<A>::operator=(WeakAssetPtr& other)
    {
        ptr = other.ptr;
        return *this;
    }

    template <Common::DerivedFrom<Asset> A>
    WeakAssetPtr<A>& WeakAssetPtr<A>::operator=(WeakAssetPtr&& other) noexcept
    {
        ptr = std::move(other.ptr);
        return *this;
    }

    template <Common::DerivedFrom<Asset> A>
    void WeakAssetPtr<A>::Reset()
    {
        ptr.Reset();
    }

    template <Common::DerivedFrom<Asset> A>
    bool WeakAssetPtr<A>::Expired() const
    {
        return ptr.Expired();
    }

    template <Common::DerivedFrom<Asset> A>
    AssetPtr<A> WeakAssetPtr<A>::Lock() const
    {
        return ptr.Lock();
    }

    template <Common::DerivedFrom<Asset> A>
    SoftAssetPtr<A>::SoftAssetPtr()
        : clazz(nullptr)
        , asset()
    {
    }

    template <Common::DerivedFrom<Asset> A>
    SoftAssetPtr<A>::SoftAssetPtr(Core::Uri inUri, const Mirror::Class& inClass)
        : uri(std::move(inUri))
        , clazz(&inClass)
        , asset()
    {
    }

    template <Common::DerivedFrom<Asset> A>
    SoftAssetPtr<A>::SoftAssetPtr(AssetPtr<A>& inAsset)
        : uri(inAsset.Uri())
        , clazz(&inAsset->GetClass())
        , asset(inAsset)
    {
    }

    template <Common::DerivedFrom<Asset> A>
    SoftAssetPtr<A>::SoftAssetPtr(const SoftAssetPtr<A>& other)
        : uri(other.uri)
        , clazz(other.clazz)
        , asset(other.asset)
    {
    }

    template <Common::DerivedFrom<Asset> A>
    SoftAssetPtr<A>::SoftAssetPtr(SoftAssetPtr<A>&& other) noexcept
        : uri(std::move(other.uri))
        , clazz(other.clazz)
        , asset(std::move(other.asset))
    {
    }

    template <Common::DerivedFrom<Asset> A>
    SoftAssetPtr<A>::~SoftAssetPtr() = default;

    template <Common::DerivedFrom<Asset> A>
    SoftAssetPtr<A>& SoftAssetPtr<A>::operator=(const std::pair<Core::Uri, const Mirror::Class&>& inUriAndClass)
    {
        uri = inUriAndClass.first;
        clazz = &inUriAndClass.second;
        asset = nullptr;
        return *this;
    }

    template <Common::DerivedFrom<Asset> A>
    SoftAssetPtr<A>& SoftAssetPtr<A>::operator=(AssetPtr<A>& inAsset)
    {
        uri = inAsset.Uri();
        clazz = &inAsset->GetClass();
        asset = inAsset;
        return *this;
    }

    template <Common::DerivedFrom<Asset> A>
    SoftAssetPtr<A>& SoftAssetPtr<A>::operator=(const SoftAssetPtr<A>& other)
    {
        uri = other.uri;
        clazz = other.clazz;
        asset = other.asset;
        return *this;
    }

    template <Common::DerivedFrom<Asset> A>
    SoftAssetPtr<A>& SoftAssetPtr<A>::operator=(SoftAssetPtr<A>&& other) noexcept
    {
        uri = std::move(other.uri);
        clazz = other.clazz;
        asset = std::move(other.asset);
        return *this;
    }

    template <Common::DerivedFrom<Asset> A>
    bool SoftAssetPtr<A>::Empty() const
    {
        return uri.Empty();
    }

    template <Common::DerivedFrom<Asset> A>
    bool SoftAssetPtr<A>::Loaded() const
    {
        return asset != nullptr;
    }

    template <Common::DerivedFrom<Asset> A>
    AssetPtr<A> SoftAssetPtr<A>::Get() const
    {
        return asset;
    }

    template <Common::DerivedFrom<Asset> A>
    void SoftAssetPtr<A>::Reset()
    {
        asset = nullptr;
    }

    template <Common::DerivedFrom<Asset> A>
    const Core::Uri& SoftAssetPtr<A>::Uri() const
    {
        return uri;
    }

    template <Common::DerivedFrom<Asset> A>
    const Mirror::Class* SoftAssetPtr<A>::Class() const
    {
        return clazz;
    }

    template <Common::DerivedFrom<Asset> A>
    AssetPtr<A> AssetManager::SyncLoad(const Core::Uri& uri, const Mirror::Class& clazz)
    {
        auto iter = weakAssetRefs.find(uri);
        if (iter != weakAssetRefs.end() && !iter->second.Expired()) {
            return iter->second.Lock().StaticCast<A>();
        }

        AssetPtr<A> result = LoadInternal<A>(uri, clazz);
        AssetPtr<Asset> tempRef = result.template StaticCast<Asset>();
        if (iter == weakAssetRefs.end()) {
            weakAssetRefs.emplace(std::make_pair(uri, WeakAssetPtr<Asset>(tempRef)));
        } else {
            iter->second = tempRef;
        }
        return result;
    }

    template <Common::DerivedFrom<Asset> A>
    void AssetManager::SyncLoadSoft(SoftAssetPtr<A>& softAssetRef, const Mirror::Class& clazz)
    {
        softAssetRef = SyncLoad<A>(softAssetRef.Uri(), clazz);
    }

    template <Common::DerivedFrom<Asset> A>
    void AssetManager::AsyncLoad(const Core::Uri& uri, const Mirror::Class& clazz, const OnAssetLoaded<A>& onAssetLoaded)
    {
        threadPool.EmplaceTask([=, this]() -> void {
            AssetPtr<A> result = nullptr;
            {
                std::unique_lock<std::mutex> lock(mutex);
                auto iter = weakAssetRefs.find(uri);
                if (iter != weakAssetRefs.end() && !iter->second.Expired()) {
                    result = iter->second.Lock().StaticCast<A>();
                }
            }

            if (result == nullptr) {
                result = LoadInternal<A>(uri, clazz);
            }

            AssetPtr<Asset> tempRef = result.template StaticCast<Asset>();
            {
                std::unique_lock<std::mutex> lock(mutex);
                auto iter = weakAssetRefs.find(uri);
                if (iter == weakAssetRefs.end()) {
                    weakAssetRefs.emplace(std::make_pair(uri, WeakAssetPtr<Asset>(tempRef)));
                } else {
                    iter->second = tempRef;
                }
            }

            onAssetLoaded(result);
        });
    }

    template <Common::DerivedFrom<Asset> A>
    void AssetManager::AsyncLoadSoft(SoftAssetPtr<A>& softAssetRef, const Mirror::Class& clazz, const OnSoftAssetLoaded<A>& onSoftAssetLoaded)
    {
        threadPool.EmplaceTask([this, softAssetRef, onSoftAssetLoaded, clazz]() -> void {
            AsyncLoad(softAssetRef.Uri(), clazz, [&](AssetPtr<A>& ref) -> void {
                softAssetRef = ref;
                onSoftAssetLoaded();
            });
        });
    }

    template <Common::DerivedFrom<Asset> A>
    void AssetManager::Save(const AssetPtr<A>& assetRef)
    {
        Assert(assetRef.Valid());
        const Core::AssetUriParser parser(assetRef.Uri());
        Common::BinaryFileSerializeStream stream(parser.Parse().Absolute().String());

        const Mirror::Any ref = assetRef->GetClass().Cast(Mirror::ForwardAsArg(*assetRef.Get()));
        ref.Serialize(stream);
    }

    template <Common::DerivedFrom<Asset> A>
    void AssetManager::SaveSoft(const SoftAssetPtr<A>& softAssetRef)
    {
        Save(softAssetRef.Uri());
    }

    template <Common::DerivedFrom<Asset> A>
    AssetPtr<A> AssetManager::LoadInternal(const Core::Uri& uri, const Mirror::Class& clazz)
    {
        const Core::AssetUriParser parser(uri);
        Common::BinaryFileDeserializeStream stream(parser.Parse().Absolute().String());

        Mirror::Any ptr = clazz.New(uri);
        ptr.Deref().Deserialize(stream);

        AssetPtr<A> result = Common::SharedPtr<A>(ptr.As<A*>());
        result->SetUri(uri);
        result->PostLoad();
        return result;
    }
}
