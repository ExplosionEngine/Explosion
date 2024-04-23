//
// Created by johnk on 2022/1/23.
//

#include <RHI/TextureView.h>

namespace RHI {
    TextureViewCreateInfo::TextureViewCreateInfo(
        TextureViewType inType, TextureViewDimension inDimension, TextureAspect inAspect, uint8_t inBaseMipLevel, uint8_t inMipLevelNum, uint8_t inBaseArrayLayer, uint8_t inArrayLayerNum)
        : type(inType)
        , dimension(inDimension)
        , aspect(inAspect)
        , baseMipLevel(inBaseMipLevel)
        , mipLevelNum(inMipLevelNum)
        , baseArrayLayer(inBaseArrayLayer)
        , arrayLayerNum(inArrayLayerNum)
    {
    }

    TextureViewCreateInfo& TextureViewCreateInfo::SetType(TextureViewType inType)
    {
        type = inType;
        return *this;
    }

    TextureViewCreateInfo& TextureViewCreateInfo::SetDimension(TextureViewDimension inDimension)
    {
        dimension = inDimension;
        return *this;
    }

    TextureViewCreateInfo& TextureViewCreateInfo::SetAspect(TextureAspect inAspect)
    {
        aspect = inAspect;
        return *this;
    }

    TextureViewCreateInfo& TextureViewCreateInfo::SetMipLevels(uint8_t inBaseMipLevel, uint8_t inMipLevelNum)
    {
        baseMipLevel = inBaseMipLevel;
        mipLevelNum = inMipLevelNum;
        return *this;
    }

    TextureViewCreateInfo& TextureViewCreateInfo::SetArrayLayers(uint8_t inBaseArrayLayer, uint8_t inArrayLayerNum)
    {
        baseArrayLayer = inBaseArrayLayer;
        arrayLayerNum = inArrayLayerNum;
        return *this;
    }

    size_t TextureViewCreateInfo::Hash() const
    {
        return Common::HashUtils::CityHash(this, sizeof(TextureViewCreateInfo));
    }

    TextureView::TextureView(const TextureViewCreateInfo& createInfo) {}

    TextureView::~TextureView() = default;
}
