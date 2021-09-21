//
// Created by Zach Lee on 2021/9/14.
//

#ifndef EXPLOSION_ASSETLIBRARY_H
#define EXPLOSION_ASSETLIBRARY_H

#include <string>
#include <Common/Asset/Asset.h>

namespace Explosion {

    class AssetLibrary {
    public:
        ~AssetLibrary() = default;

        static AssetLibrary* GetAssetLibrary();

        template <typename T>
        Asset<T>* CreateAsset(const std::string& path);

    private:
        AssetLibrary() = default;

        std::unordered_map<std::string, AssetBase*> assets;
    };
}

#endif//EXPLOSION_ASSETLIBRARY_H
