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
#include <Core/Uri.h>
#include <Mirror/Meta.h>
#include <Mirror/Mirror.h>
#include <Runtime/Api.h>

namespace Runtime {
    struct EClass() Asset {
        EClassBody(Asset)

        ECtor()
        Asset() : uri()
        {
        }

        ECtor()
        explicit Asset(Core::Uri inUri) : uri(std::move(inUri))
        {
        }

        EProperty()
        Core::Uri uri;
    };

    template <typename A>
    requires std::is_base_of_v<Asset, A>
    class AssetRef {
    public:
        template <typename A2> AssetRef(Common::SharedRef<A2>& inRef) : ref(inRef) {} // NOLINT
        template <typename A2> AssetRef(Common::SharedRef<A2>&& inRef) noexcept : ref(std::move(inRef)) {} // NOLINT
        AssetRef(A* pointer) : ref(pointer) {} // NOLINT
        AssetRef(AssetRef& other) : ref(other.ref) {} // NOLINT
        AssetRef(AssetRef&& other) noexcept : ref(std::move(other.ref)) {} // NOLINT
        AssetRef() = default;
        ~AssetRef() = default;

        template <typename A2>
        AssetRef& operator=(Common::SharedRef<A2>& inRef)
        {
            ref = inRef;
            return *this;
        }

        template <typename A2>
        AssetRef& operator=(Common::SharedRef<A2>&& inRef) noexcept
        {
            ref = inRef;
            return *this;
        }

        AssetRef& operator=(A* pointer)
        {
            ref = pointer;
            return *this;
        }

        AssetRef& operator=(AssetRef& other) // NOLINT
        {
            ref = other.ref;
            return *this;
        }

        AssetRef& operator=(AssetRef&& other) noexcept
        {
            ref = std::move(other.ref);
            return *this;
        }

        const Core::Uri& Uri() const
        {
            Assert(ref != nullptr);
            return ref->uri;
        }

        A* operator->() const noexcept
        {
            return ref.operator->();
        }

        A& operator*() const noexcept
        {
            return ref.operator*();
        }

        bool operator==(nullptr_t) const noexcept
        {
            return ref == nullptr;
        }

        bool operator!=(nullptr_t) const noexcept
        {
            return ref != nullptr;
        }

        A* Get() const
        {
            return ref.Get();
        }

        void Reset(A* pointer = nullptr)
        {
            ref.Reset(pointer);
        }

        auto RefCount() const
        {
            return ref.RefCount();
        }

        Common::SharedRef<A>& GetSharedRef()
        {
            return ref;
        }

        template <typename A2>
        AssetRef<A2> StaticCast()
        {
            return ref.template StaticCast<A2>();
        }

        template <typename A2>
        AssetRef<A2> DynamicCast()
        {
            return ref.template DynamicCast<A2>();
        }

        template <typename A2>
        AssetRef<A2> ReinterpretCast()
        {
            return ref.template ReinterpretCast<A2>();
        }

    private:
        Common::SharedRef<A> ref;
    };

    template <typename A>
    requires std::is_base_of_v<Asset, A>
    class WeakAssetRef {
    public:
        template <typename A2> WeakAssetRef(AssetRef<A2>& inRef) : ref(inRef.GetSharedRef()) {} // NOLINT
        WeakAssetRef(WeakAssetRef& other) : ref(other.ref) {} // NOLINT
        WeakAssetRef(WeakAssetRef&& other) noexcept : ref(std::move(other.ref)) {} // NOLINT

        template <typename A2>
        WeakAssetRef& operator=(AssetRef<A2>& inRef)
        {
            ref = inRef.GetSharedRef();
            return *this;
        }

        WeakAssetRef& operator=(WeakAssetRef& other) // NOLINT
        {
            ref = other.ref;
            return *this;
        }

        WeakAssetRef& operator=(WeakAssetRef&& other) noexcept
        {
            ref = std::move(other.ref);
            return *this;
        }

        void Reset()
        {
            ref.Reset();
        }

        bool Expired() const
        {
            return ref.Expired();
        }

        AssetRef<A> Lock() const
        {
            return ref.Lock();
        }

    private:
        Common::WeakRef<A> ref;
    };

    template <typename A>
    class SoftAssetRef {
    public:
        SoftAssetRef()
            : uri()
            , asset()
        {
        }

        explicit SoftAssetRef(Core::Uri inUri)
            : uri(std::move(inUri))
            , asset()
        {
        }

        explicit SoftAssetRef(AssetRef<A>& inAsset)
            : uri(inAsset.Uri())
            , asset(inAsset)
        {
        }

        SoftAssetRef(const SoftAssetRef<A>& other)
            : uri(other.uri)
            , asset(other.asset)
        {
        }

        SoftAssetRef(SoftAssetRef<A>&& other) noexcept
            : uri(std::move(other.uri))
            , asset(std::move(other.asset))
        {
        }

        ~SoftAssetRef() = default;

        SoftAssetRef& operator=(Core::Uri inUri)
        {
            uri = std::move(inUri);
            asset = nullptr;
            return *this;
        }

        SoftAssetRef& operator=(AssetRef<A>& inAsset)
        {
            uri = inAsset.Uri();
            asset = inAsset;
            return *this;
        }

        SoftAssetRef& operator=(const SoftAssetRef<A>& other)
        {
            uri = other.uri;
            asset = other.asset;
            return *this;
        }

        SoftAssetRef& operator=(SoftAssetRef<A>&& other) noexcept
        {
            uri = std::move(other.uri);
            asset = std::move(other.asset);
            return *this;
        }

        bool Empty() const
        {
            return uri.Empty();
        }

        bool Loaded() const
        {
            return asset != nullptr;
        }

        AssetRef<A> Get() const
        {
            return asset;
        }

        void Reset()
        {
            asset = nullptr;
        }

        const Core::Uri& Uri() const
        {
            return uri;
        }

    private:
        Core::Uri uri;
        AssetRef<A> asset;
    };

    template <typename A>
    using OnAssetLoaded = std::function<void(AssetRef<A>)>;

    template <typename A>
    using OnSoftAssetLoaded = std::function<void()>;

    class RUNTIME_API AssetManager {
    public:
        static AssetManager& Get();
        ~AssetManager();

        template <typename A>
        AssetRef<A> SyncLoad(const Core::Uri& uri)
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

        template <typename A>
        void SyncLoadSoft(SoftAssetRef<A>& softAssetRef)
        {
            softAssetRef = SyncLoad<A>(softAssetRef.Uri());
        }

        template <typename A>
        void AsyncLoad(const Core::Uri& uri, const OnAssetLoaded<A>& onAssetLoaded)
        {
            threadPool.EmplaceTask([this, uri, onAssetLoaded]() -> void {
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

        template <typename A>
        void AsyncLoadSoft(SoftAssetRef<A>& softAssetRef, const OnSoftAssetLoaded<A>& onSoftAssetLoaded)
        {
            threadPool.EmplaceTask([this, softAssetRef, onSoftAssetLoaded]() -> void {
                AsyncLoad(softAssetRef.Uri(), [&](AssetRef<A>& ref) -> void {
                    softAssetRef = ref;
                    onSoftAssetLoaded();
                });
            });
        }

        template <typename A>
        void Save(const AssetRef<A>& assetRef)
        {
            if (assetRef == nullptr) {
                return;
            }

            Core::AssetUriParser parser(assetRef.Uri());
            auto pathString = parser.AbsoluteFilePath().string();
            Common::BinaryFileSerializeStream stream(pathString);

            Mirror::Any ref = std::ref(*assetRef.Get());
            A::GetClass().Serialize(stream, &ref);
        }

        template <typename A>
        void SaveSoft(const SoftAssetRef<A>& softAssetRef)
        {
            Save(softAssetRef.Uri());
        }

    private:
        template <typename A>
        AssetRef<A> LoadInternal(const Core::Uri& uri)
        {
            Core::AssetUriParser parser(uri);
            auto pathString = parser.AbsoluteFilePath().string();
            Common::BinaryFileDeserializeStream stream(pathString);

            AssetRef<A> result = Common::SharedRef<A>(new A());
            Mirror::Any ref = std::ref(*result.Get());
            A::GetClass().Deserailize(stream, &ref);

            // reset uri is useful for moved asset
            result->uri = uri;
            return result;
        }

        AssetManager();

        std::mutex mutex;
        std::unordered_map<Core::Uri, WeakAssetRef<Asset>> weakAssetRefs;
        Common::ThreadPool threadPool;
    };
}

namespace Common {
    template <typename A>
    requires std::is_base_of_v<Runtime::Asset, A>
    struct Serializer<Runtime::AssetRef<A>> {
        static constexpr bool serializable = true;
        static constexpr uint32_t typeId = Common::HashUtils::StrCrc32("Runtime::AssetRef");

        static void Serialize(SerializeStream& stream, const Runtime::AssetRef<A>& value)
        {
            TypeIdSerializer<Runtime::AssetRef<A>>::Serialize(stream);
            Serializer<Core::Uri>::Serialize(stream, value);
        }

        static bool Deserialize(DeserializeStream& stream, Runtime::AssetRef<A>& value)
        {
            if (!TypeIdSerializer<Runtime::AssetRef<A>>::Deserialize(stream)) {
                return false;
            }

            Core::Uri uri;
            Serializer<Core::Uri>::Deserialize(stream, uri);
            value = Runtime::AssetManager::Get().SyncLoad<A>(uri, uri);
            return true;
        }
    };

    template <typename A>
    requires std::is_base_of_v<Runtime::Asset, A>
    struct Serializer<Runtime::SoftAssetRef<A>> {
        static constexpr bool serializable = true;
        static constexpr uint32_t typeId = Common::HashUtils::StrCrc32("Runtime::SoftAssetRef");

        static void Serialize(SerializeStream& stream, const Runtime::SoftAssetRef<A>& value)
        {
            TypeIdSerializer<Runtime::AssetRef<A>>::Serialize(stream);
            Serializer<Core::Uri>::Serialize(stream, value);
        }

        static bool Deserialize(DeserializeStream& stream, Runtime::SoftAssetRef<A>& value)
        {
            if (!TypeIdSerializer<Runtime::AssetRef<A>>::Deserialize(stream)) {
                return false;
            }

            Core::Uri uri;
            Serializer<Core::Uri>::Deserialize(stream, uri);
            value = uri;
            return true;
        }
    };
}
