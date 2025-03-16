//
// Created by johnk on 2022/1/23.
//

#pragma once

#include <Common/Utility.h>
#include <Common/Hash.h>
#include <RHI/Common.h>

namespace RHI {
    struct TextureViewCreateInfo {
        TextureViewType type;
        TextureViewDimension dimension;
        TextureAspect aspect;
        uint8_t baseMipLevel;
        uint8_t mipLevelNum;
        uint8_t baseArrayLayer;
        uint8_t arrayLayerNum;

        explicit TextureViewCreateInfo(
            TextureViewType inType = TextureViewType::max,
            TextureViewDimension inDimension = TextureViewDimension::max,
            TextureAspect inAspect = TextureAspect::color,
            uint8_t inBaseMipLevel = 0,
            uint8_t inMipLevelNum = 1,
            uint8_t inBaseArrayLayer = 0,
            uint8_t inArrayLayerNum = 1);

        TextureViewCreateInfo& SetType(TextureViewType inType);
        TextureViewCreateInfo& SetDimension(TextureViewDimension inDimension);
        TextureViewCreateInfo& SetAspect(TextureAspect inAspect);
        TextureViewCreateInfo& SetMipLevels(uint8_t inBaseMipLevel, uint8_t inMipLevelNum);
        TextureViewCreateInfo& SetArrayLayers(uint8_t inBaseArrayLayer, uint8_t inArrayLayerNum);

        uint64_t Hash() const;
    };

    class TextureView {
    public:
        NonCopyable(TextureView)
        virtual ~TextureView();

    protected:
        explicit TextureView(const TextureViewCreateInfo& createInfo);
    };
}
