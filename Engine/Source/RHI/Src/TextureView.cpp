//
// Created by johnk on 2022/1/23.
//

#include <RHI/TextureView.h>

namespace RHI {
    TextureViewCreateInfo::TextureViewCreateInfo()
        : type(TextureViewType::max)
        , dimension(TextureViewDimension::max)
        , aspect(TextureAspect::max)
        , baseMipLevel(0)
        , mipLevelNum(0)
        , baseArrayLayer(0)
        , arrayLayerNum(0)
    {
    }

    TextureViewCreateInfo& TextureViewCreateInfo::Type(TextureViewType inType)
    {
        type = inType;
        return *this;
    }

    TextureViewCreateInfo& TextureViewCreateInfo::Dimension(TextureViewDimension inDimension)
    {
        dimension = inDimension;
        return *this;
    }

    TextureViewCreateInfo& TextureViewCreateInfo::Aspect(TextureAspect inAspect)
    {
        aspect = inAspect;
        return *this;
    }

    TextureViewCreateInfo& TextureViewCreateInfo::MipLevels(uint8_t inBaseMipLevel, uint8_t inMipLevelNum)
    {
        baseMipLevel = inBaseMipLevel;
        mipLevelNum = inMipLevelNum;
        return *this;
    }

    TextureViewCreateInfo& TextureViewCreateInfo::ArrayLayers(uint8_t inBaseArrayLayer, uint8_t inArrayLayerNum)
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
