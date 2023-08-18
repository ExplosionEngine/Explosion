//
// Created by johnk on 2023/4/4.
//

#include <utility>

#include <Runtime/Asset.h>

namespace Runtime {
    AssetManager::AssetManager()
        : threadPool("AssetLoadingThread", 4)
    {
    }

    AssetManager::~AssetManager() = default;
}
