//
// Created by johnk on 2022/1/23.
//

#ifndef EXPLOSION_RHI_TEXTURE_VIEW_H
#define EXPLOSION_RHI_TEXTURE_VIEW_H

#include <Common/Utility.h>
#include <RHI/Enum.h>

namespace RHI {
    struct TextureViewCreateInfo {
        PixelFormat format;
        TextureViewDimension dimension;
        TextureAspect aspect;
        size_t baseMipLevel;
        size_t mipLevelNum;
        size_t baseArrayLayer;
        size_t arrayLayerNum;
    };

    class TextureView {
    public:
        NON_COPYABLE(TextureView)
        virtual ~TextureView();

        virtual void Destroy() = 0;

    protected:
        explicit TextureView(const TextureViewCreateInfo* createInfo);
    };
}

#endif //EXPLOSION_RHI_TEXTURE_VIEW_H
