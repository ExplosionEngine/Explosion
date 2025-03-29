//
// Created by johnk on 2023/10/10.
//

#include <Runtime/Asset/Asset.h>

namespace Runtime {
    Asset::Asset() = default;

    Asset::Asset(Core::Uri inUri)
        : uri(std::move(inUri))
    {
    }

    Asset::~Asset() = default;

    const Core::Uri& Asset::Uri() const
    {
        return uri;
    }

    void Asset::SetUri(Core::Uri inUri)
    {
        uri = std::move(inUri);
    }

    void Asset::PostLoad() {}

    AssetManager& AssetManager::Get()
    {
        static AssetManager instance;
        return instance;
    }

    AssetManager::AssetManager()
        : threadPool("AssetThreadPool", 4)
    {
    }

    AssetManager::~AssetManager() = default;
}
