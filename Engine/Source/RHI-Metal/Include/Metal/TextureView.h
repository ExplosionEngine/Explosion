//
// Created by Zach Lee on 2022/10/1.
//

#pragma once

#include <RHI/TextureView.h>
#import <Metal/MTLTexture.h>
#import <Metal/Metal.h>

namespace RHI::Metal {
    class MTLTexture;

    class MTLTextureView : public TextureView {
    public:
        MTLTextureView(MTLTexture &texture, const TextureViewCreateInfo* createInfo);
        ~MTLTextureView();

        void Destroy() override;
    private:
        void CreateNativeTextureView(MTLTexture &texture, const TextureViewCreateInfo *createInfo);

        id<MTLTexture> textureView;
    };
}
