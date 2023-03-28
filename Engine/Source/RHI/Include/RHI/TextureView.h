//
// Created by johnk on 2022/1/23.
//

#pragma once

#include <Common/Utility.h>
#include <RHI/Common.h>

namespace RHI {
    struct TextureViewCreateInfo {
        TextureViewDimension dimension;
        TextureAspect aspect;
        uint8_t baseMipLevel;
        uint8_t mipLevelNum;
        uint8_t baseArrayLayer;
        uint8_t arrayLayerNum;
    };

    class TextureView {
    public:
        NON_COPYABLE(TextureView)
        virtual ~TextureView();

        virtual void Destroy() = 0;

    protected:
        explicit TextureView(const TextureViewCreateInfo& createInfo);
    };
}
