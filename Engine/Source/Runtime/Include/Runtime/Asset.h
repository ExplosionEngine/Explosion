//
// Created by johnk on 2023/10/10.
//

#pragma once

#include <string>
#include <functional>
#include <unordered_map>

#include <Common/Memory.h>
#include <Common/Serialization.h>
#include <Core/Uri.h>

namespace Runtime {
    struct Asset {
        Core::Uri uri;
    };

    template <typename A>
    class AssetRef {
    public:
        // TODO
    };

    template <typename A>
    class WeakAssetRef {
    public:
        // TODO
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

    private:
        Core::Uri uri;
        AssetRef<A> asset;
    };

    template <typename A>
    using OnAssetLoaded = std::function<void(AssetRef<A>)>;

    template <typename A>
    using OnSoftAssetLoaded = std::function<void()>;

    class AssetManager {
    public:
        static AssetManager& Get();
        ~AssetManager();

        template <typename A>
        AssetRef<A> SyncLoad(const Core::Uri& uri);

        template <typename A>
        void AsyncLoad(const Core::Uri& uri, const OnAssetLoaded<A>& onAssetLoaded);

        template <typename A>
        void SyncLoadSoft(SoftAssetRef<A>& softAssetRef);

        template <typename A>
        void AsyncLoadSoft(const Core::Uri& uri, const OnSoftAssetLoaded<A>& onSoftAssetLoaded);

        template <typename A>
        void Save(const AssetRef<A>& assetRef);

        template <typename A>
        void SaveSoft(const SoftAssetRef<A>& softAssetRef);

    private:
        AssetManager();

        std::unordered_map<Core::Uri, Common::WeakRef<Asset>> weakAssetRefs;
    };
}

namespace Common {
    template <typename A>
    requires std::is_base_of_v<Runtime::Asset, A>
    struct Serializer<Runtime::AssetRef<A>> {
        static constexpr bool serializable = true;
        static constexpr uint32_t typeId = Common::HashUtils::StrCrc32("assetRef");

        static void Serialize(SerializeStream& stream, const Runtime::AssetRef<A>& value)
        {
            TypeIdSerializer<Runtime::AssetRef<A>>::Serialize(stream);
            // TODO
        }

        static bool Deserialize(DeserializeStream& stream, Runtime::AssetRef<A>& value)
        {
            if (!TypeIdSerializer<Runtime::AssetRef<A>>::Deserialize(stream)) {
                return false;
            }

            // TODO
            return true;
        }
    };
}
