//
// Created by johnk on 2022/1/23.
//

#include <RHI/TextureView.h>

namespace RHI {
    TextureViewCreateInfo::TextureViewCreateInfo(
        const TextureViewType inType,
        const TextureViewDimension inDimension,
        const TextureAspect inAspect,
        const uint8_t inBaseMipLevel,
        const uint8_t inMipLevelNum,
        const uint8_t inBaseArrayLayer,
        const uint8_t inArrayLayerNum)
        : type(inType)
        , dimension(inDimension)
        , aspect(inAspect)
        , baseMipLevel(inBaseMipLevel)
        , mipLevelNum(inMipLevelNum)
        , baseArrayLayer(inBaseArrayLayer)
        , arrayLayerNum(inArrayLayerNum)
    {
    }

    TextureViewCreateInfo& TextureViewCreateInfo::SetType(const TextureViewType inType)
    {
        type = inType;
        return *this;
    }

    TextureViewCreateInfo& TextureViewCreateInfo::SetDimension(const TextureViewDimension inDimension)
    {
        dimension = inDimension;
        return *this;
    }

    TextureViewCreateInfo& TextureViewCreateInfo::SetAspect(const TextureAspect inAspect)
    {
        aspect = inAspect;
        return *this;
    }

    TextureViewCreateInfo& TextureViewCreateInfo::SetMipLevels(const uint8_t inBaseMipLevel, const uint8_t inMipLevelNum)
    {
        baseMipLevel = inBaseMipLevel;
        mipLevelNum = inMipLevelNum;
        return *this;
    }

    TextureViewCreateInfo& TextureViewCreateInfo::SetArrayLayers(const uint8_t inBaseArrayLayer, const uint8_t inArrayLayerNum)
    {
        baseArrayLayer = inBaseArrayLayer;
        arrayLayerNum = inArrayLayerNum;
        return *this;
    }

    size_t TextureViewCreateInfo::Hash() const
    {
        return Common::HashUtils::CityHash(this, sizeof(TextureViewCreateInfo));
    }

    TextureView::TextureView(const TextureViewCreateInfo&) {}

    TextureView::~TextureView() = default;
}
