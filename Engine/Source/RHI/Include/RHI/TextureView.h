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

        TextureViewCreateInfo();
        TextureViewCreateInfo& Type(TextureViewType inType);
        TextureViewCreateInfo& Dimension(TextureViewDimension inDimension);
        TextureViewCreateInfo& Aspect(TextureAspect inAspect);
        TextureViewCreateInfo& MipLevels(uint8_t inBaseMipLevel, uint8_t inMipLevelNum);
        TextureViewCreateInfo& ArrayLayers(uint8_t inBaseArrayLayer, uint8_t inArrayLayerNum);

        size_t Hash() const;
    };

    class TextureView {
    public:
        NonCopyable(TextureView)
        virtual ~TextureView();

        virtual void Destroy() = 0;

    protected:
        explicit TextureView(const TextureViewCreateInfo& createInfo);
    };
}
