//
// Created by Zach Lee on 2022/10/1.
//

#pragma once

#include <RHI/Texture.h>
#include <Metal/TextureView.h>
#import <QuartzCore/CAMetalLayer.h>
#import <Metal/Metal.h>
#import <Metal/MTLTexture.h>

namespace RHI::Metal {
    class MTLDevice;

    class MTLTexture : public Texture {
    public:
        MTLTexture(MTLDevice &device, const TextureCreateInfo* createInfo);
        ~MTLTexture();

        id<MTLTexture> GetNativeTexture() const;

        void SetDrawable(id<CAMetalDrawable> value);

        void ResetDrawable();

        bool IsDrawableTexture() const;

        TextureView* CreateTextureView(const TextureViewCreateInfo* createInfo) override;
        void Destroy() override;
    private:
        MTLDevice &device;
        id<MTLTexture> texture = nil;
        DrawablePtr drawable;
    };
}
