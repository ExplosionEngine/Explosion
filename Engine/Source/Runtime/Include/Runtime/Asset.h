//
// Created by johnk on 2023/4/4.
//

#pragma once

#include <string>
#include <future>
#include <utility>
#include <unordered_set>
#include <fstream>

#include <Mirror/Meta.h>
#include <Mirror/Type.h>
#include <Common/Concurrent.h>
#include <Common/Memory.h>

namespace Runtime {
    using Uri = std::string;

    class EClass() Asset {
    public:
        EClassBody(Asset)

        EProperty()
        Uri uri;
    };

    template <typename A>
    using AssetRef = Common::SharedRef<A>;

    class SoftRefBasic {
    protected:
        explicit SoftRefBasic(Uri inUri) : uri(std::move(inUri)) {}
        ~SoftRefBasic() = default;

        Uri uri;

    private:
        friend class AssetManager;
        friend class AssetSerializeContext;
    };

    template <typename A>
    class SoftRef : public SoftRefBasic {
    public:
        SoftRef(Uri inUri) : SoftRefBasic(std::move(inUri)) {} // NOLINT
        SoftRef(AssetRef<A>& inAsset) : SoftRefBasic(inAsset.uri), liveAsset(inAsset) {} // NOLINT
        SoftRef(AssetRef<A>&& inAsset) noexcept : SoftRefBasic(inAsset.uri), liveAsset(std::move(inAsset)) {} // NOLINT
        SoftRef(SoftRef<A>& inRef) : SoftRefBasic(inRef.uri), liveAsset(inRef.liveAsset) {} // NOLINT
        SoftRef(SoftRef<A>&& inAsset) noexcept : SoftRefBasic(std::move(inAsset.uri)), liveAsset(std::move(inAsset.liveAsset)) {} // NOLINT

        SoftRef& operator=(const Uri& inUri)
        {
            uri = inUri;
            return *this;
        }

        SoftRef& operator=(AssetRef<A>& inAsset)
        {
            uri = inAsset.uri;
            liveAsset = inAsset;
            return *this;
        }

        SoftRef& operator=(AssetRef<A>&& inAsset) noexcept
        {
            uri = std::move(inAsset.uri);
            liveAsset = std::move(inAsset);
            return *this;
        }

        SoftRef& operator=(SoftRef<A>& inRef) // NOLINT
        {
            uri = inRef.uri;
            liveAsset = inRef.liveAsset;
            return *this;
        }

        SoftRef& operator=(SoftRef<A>&& inRef) noexcept
        {
            uri = std::move(inRef.uri);
            liveAsset = std::move(inRef.liveAsset);
            return *this;
        }

        A* operator->() const noexcept
        {
            Assert(liveAsset != nullptr);
            return liveAsset.operator->();
        }

        A& operator*() const noexcept
        {
            Assert(liveAsset != nullptr);
            return liveAsset.operator*();
        }

        bool operator==(nullptr_t) const noexcept
        {
            return liveAsset == nullptr;
        }

        bool operator!=(nullptr_t) const noexcept
        {
            return liveAsset != nullptr;
        }

        A* Get() const
        {
            Assert(liveAsset != nullptr);
            return liveAsset.get();
        }

        void Reset()
        {
            liveAsset.reset();
        }

        auto RefCount() const
        {
            Assert(liveAsset != nullptr);
            return liveAsset.use_count();
        }

    private:
        friend class AssetManager;

        AssetRef<A> liveAsset;
    };

    class AssetManager {
    public:
        ~AssetManager();

        template <typename A>
        AssetRef<A> LoadOrGetSync(const Uri& uri)
        {
            return GetFromMemoryOrReadFromFile<A>(uri);
        }

        template <typename A>
        void LoadOrGetSync(SoftRef<A>& softRef)
        {
            softRef.liveAsset = GetFromMemoryOrReadFromFile<A>(softRef.uri);
        }

        template <typename A>
        std::future<AssetRef<A>> LoadOrGetAsync(const Uri& uri)
        {
            return threadPool.EmplaceTask([this, &uri]() -> auto {
                return GetFromMemoryOrReadFromFile<A>(uri);
            });
        }

        template <typename A, typename F>
        void LoadOrGetAsync(const Uri& uri, F&& onLoadOver)
        {
            threadPool.EmplaceTask([this, &uri, &onLoadOver]() -> void {
                onLoadOver(GetFromMemoryOrReadFromFile<A>(uri));
            });
        }

        template <typename A, typename F>
        std::future<void> LoadOrGetAsync(SoftRef<A>& softRef)
        {
            return threadPool.EmplaceTask([this, &softRef]() -> void {
                softRef.liveAsset = GetFromMemoryOrReadFromFile<A>(softRef.uri);
            });
        }

        template <typename A, typename F>
        void LoadOrGetAsync(SoftRef<A>& softRef, F&& onLoadOver)
        {
            threadPool.EmplaceTask([this, &softRef, &onLoadOver]() -> void {
                softRef.liveAsset = GetFromMemoryOrReadFromFile<A>(softRef.uri);
                onLoadOver();
            });
        }

        template <typename A>
        void SaveSync(AssetRef<A>& asset)
        {
            WriteToFile(asset);
        }

        template <typename A>
        void SaveSync(SoftRef<A>& softRef)
        {
            Assert(softRef.liveAsset != nullptr);
            WriteToFile(softRef);
        }

        template <typename A>
        std::future<void> SaveAsync(AssetRef<A>& asset)
        {
            return threadPool.EmplaceTask([this, &asset]() -> void {
                WriteToFile(asset);
            });
        }

        template <typename A, typename F>
        void SaveAsync(AssetRef<A>& asset, F&& onSaveOver)
        {
            threadPool.EmplaceTask([this, &asset, &onSaveOver]() -> void {
                WriteToFile(asset);
                onSaveOver();
            });
        }

        template <typename A>
        std::future<void> SaveAsync(SoftRef<A>& softRef)
        {
            Assert(softRef.liveAsset != nullptr);
            return threadPool.EmplaceTask([this, &softRef]() -> void {
                WriteToFile(softRef.liveAsset);
            });
        }

        template <typename A, typename F>
        void SaveAsync(SoftRef<A>& softRef, F&& onSaveOver)
        {
            Assert(softRef.liveAsset != nullptr);
            threadPool.EmplaceTask([this, &softRef, &onSaveOver]() -> void {
                WriteToFile(softRef.liveAsset);
                onSaveOver();
            });
        }

    private:
        struct AssetFileElementAlign {
            size_t nameSize;
            size_t typeNameSize;
            size_t metaDataSize;
            size_t memorySize;
        };

        template <typename A>
        AssetRef<A> GetFromMemoryOrReadFromFile(const Uri& uri)
        {
            auto iter = assets.find(uri);
            if (iter == assets.end() || iter->second.Expired()) {
                auto ref = ReadFromFile<A>(uri);
                assets[uri] = ref;
                return ref;
            }
            return iter->second.Lock().StaticCast<A>();
        }

        template <typename A>
        AssetRef<A> ReadFromFile(const Uri& uri)
        {
            // TODO
        }

        template <typename A>
        void WriteToFile(AssetRef<A>& ref)
        {
            // TODO
        }

        friend class Engine;

        explicit AssetManager();

        Common::ThreadPool threadPool;
        std::unordered_map<Uri, Common::WeakRef<Asset>> assets;
    };
}
