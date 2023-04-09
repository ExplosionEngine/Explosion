//
// Created by johnk on 2023/4/4.
//

#pragma once

#include <string>
#include <future>

#include <Mirror/Meta.h>
#include <Common/Path.h>
#include <Common/Concurrent.h>

namespace Runtime {
    class Uri {
    public:
        Uri(std::string inUri); // NOLINT
        Uri(Uri&& other) noexcept;
        ~Uri() = default;

        const std::string& ToString() const;
        std::string ToAbsolutePath(const Common::PathMapper& pathMapper) const;

    private:
        std::string uri;
    };

    class EClass() Asset {
    public:
        EProperty()
        Uri uri;
    };

    class AssetManager {
    public:
        ~AssetManager();

        template <typename A>
        std::future<Asset*> LoadOrGet(const Uri& uri)
        {
            // TODO
            return {};
        }

        template <typename A, typename F>
        void LoadOrGet(const Uri& uri, F&& func)
        {
            // TODO
        }

        template <typename A>
        void Unload(A&& asset)
        {
            // TODO
        }

    private:
        friend class Engine;

        explicit AssetManager(const Common::PathMapper& inPathMapper);

        const Common::PathMapper& pathMapper;
        Common::ThreadPool threadPool;
        // TODO use set instead ?
        mutable std::vector<std::unique_ptr<Asset>> assets;
    };
}
