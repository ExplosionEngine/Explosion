//
// Created by johnk on 2023/10/10.
//

#include <Runtime/Asset.h>

namespace Runtime {
    Asset::Asset() = default;

    Asset::Asset(Core::Uri inUri)
        : uri(std::move(inUri))
    {
    }

    AssetManager& AssetManager::Get()
    {
        static AssetManager instance;
        return instance;
    }

    AssetManager::AssetManager()
        : weakAssetRefs()
        , threadPool("AssetThreadPool", 4)
    {
    }

    AssetManager::~AssetManager() = default;
}
