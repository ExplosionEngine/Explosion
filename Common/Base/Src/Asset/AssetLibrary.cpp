//
// Created by Zach Lee on 2021/9/9.
//

#include <Common/Asset/AssetLibrary.h>

namespace Explosion {

    AssetLibrary* AssetLibrary::GetAssetLibrary()
    {
        static AssetLibrary library;
        return &library;
    }

}