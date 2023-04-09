//
// Created by johnk on 2023/4/4.
//

#include <utility>

#include <Runtime/Asset.h>

namespace Runtime {
    Uri::Uri(std::string inUri)
        : uri(std::move(inUri))
    {
    }

    Uri::Uri(Uri&& other) noexcept
        : uri(std::move(other.uri))
    {
    }

    const std::string& Uri::ToString() const
    {
        return uri;
    }

    std::string Uri::ToAbsolutePath(const Common::PathMapper& pathMapper) const
    {
        // TODO
        return "";
    }

    AssetManager::AssetManager(const Common::PathMapper& inPathMapper)
        : pathMapper(inPathMapper)
        , threadPool("AssetThread", 4)
    {
    }

    AssetManager::~AssetManager() = default;
}
