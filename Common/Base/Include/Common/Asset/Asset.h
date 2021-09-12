//
// Created by LiZhen on 2021/9/9.
//

#ifndef EXPLOSION_ASSET_H
#define EXPLOSION_ASSET_H

#include <string>
#include <vector>
#include <unordered_map>
#include <Common/NonCopy.h>

namespace Explosion {

    struct AssetData {
        std::string type;
        uint32_t version;
        std::vector<uint8_t> rawData;
    };

    class AssetBase {
    public:
        virtual ~AssetBase() = default;
        EXPLOSION_NON_COPY(AssetBase)

        enum Status : uint8_t {
            UNLOAD,
            LOADING,
            LOADED
        };

        virtual void Load() {}

        virtual void UnLoad() {}

        virtual void BlockUntilLoaded() {}

    protected:
        AssetBase() = default;

        std::string path;
        Status status;
        AssetData data;
    };

    template <typename T>
    class Asset : public AssetBase {
    public:
        using SourceType = T;

        Asset() = default;
        ~Asset() = default;

    protected:
        friend class Asset;
    };
}

#endif//EXPLOSION_ASSET_H
