//
// Created by Zach Lee on 2022/10/1.
//

#pragma once

#include <memory>
#include <RHI/TextureView.h>
#import <QuartzCore/CAMetalLayer.h>
#import <Metal/MTLTexture.h>
#import <Metal/Metal.h>

namespace RHI::Metal {
    class MTLTexture;

    class DrawableProvider {
    public:
        DrawableProvider() = default;
        ~DrawableProvider() = default;

        id<MTLTexture> GetTexture() const
        {
            return drawable.texture;
        }

        void SetDrawable(id<CAMetalDrawable> value)
        {
            drawable = value;
            [drawable retain];
        }

        void Reset()
        {
            [drawable release];
            drawable = nil;
        }

    private:
        id<CAMetalDrawable> drawable = nil;
    };
    using DrawablePtr = std::shared_ptr<DrawableProvider>;

    class MTLTextureView : public TextureView {
    public:
        MTLTextureView(MTLTexture &texture, const TextureViewCreateInfo& createInfo);
        ~MTLTextureView();

        void Destroy() override;

        id<MTLTexture> GetTexture() const;

        void SetDrawable(const DrawablePtr &value);
    private:
        void CreateNativeTextureView(MTLTexture &texture, const TextureViewCreateInfo& createInfo);

        id<MTLTexture> textureView;
        DrawablePtr drawable;
    };
}
