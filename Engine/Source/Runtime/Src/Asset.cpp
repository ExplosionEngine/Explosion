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

    AssetManager::AssetManager() = default;

    AssetManager::~AssetManager() = default;
}
