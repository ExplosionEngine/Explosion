//
// Created by johnk on 2023/4/4.
//

#pragma once

#include <string>
#include <future>
#include <utility>
#include <unordered_set>

#include <Mirror/Meta.h>
#include <Mirror/Type.h>
#include <Common/Path.h>
#include <Common/Concurrent.h>
#include <Common/Memory.h>

namespace Runtime {
    using Uri = std::string;

    class EClass() Asset {
    public:
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
        AssetRef<A> LoadSync(const Uri& uri)
        {
            // TODO
        }

        template <typename A>
        void LoadSync(SoftRef<A>& softRef)
        {
            // TODO
        }

        template <typename A>
        std::future<AssetRef<A>> LoadAsync(const Uri& uri)
        {
            // TODO
            return {};
        }

        template <typename A, typename F>
        void LoadAsync(const Uri& uri, F&& onLoadOver)
        {
            // TODO
        }

        template <typename A, typename F>
        std::future<void> LoadAsync(SoftRef<A>& softRef)
        {
            // TODO
        }

        template <typename A, typename F>
        void LoadAsync(SoftRef<A>& softRef, F&& onLoadOver)
        {
            // TODO
        }

        template <typename A>
        void Save(AssetRef<A>& asset)
        {
            // TODO
        }

    private:
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

        explicit AssetManager(const Common::PathMapper& inPathMapper);

        const Common::PathMapper& pathMapper;
        Common::ThreadPool threadPool;
    };
}
