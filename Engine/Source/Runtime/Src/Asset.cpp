//
// Created by johnk on 2023/10/10.
//

#include <Runtime/Asset.h>

namespace Runtime {
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
