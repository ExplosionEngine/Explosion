//
// Created by Zach Lee on 2021/9/14.
//

#ifndef EXPLOSION_ASSETINSTANCE_H
#define EXPLOSION_ASSETINSTANCE_H

namespace Explosion {

    template <typename T>
    class AssetInstance {
    public:
        AssetInstance() = default;
        ~AssetInstance() = default;

    private:
        uint32_t id;
        T* resource;
    };

}

#endif//EXPLOSION_ASSETINSTANCE_H
