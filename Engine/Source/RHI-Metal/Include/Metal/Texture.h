//
// Created by Zach Lee on 2022/10/1.
//

#pragma once

#include <RHI/Texture.h>
#import <Metal/Metal.h>
#import <Metal/MTLTexture.h>

namespace RHI::Metal {
    class MTLDevice;

    class MTLTexture : public Texture {
    public:
        MTLTexture(MTLDevice &device, const TextureCreateInfo* createInfo);
        ~MTLTexture();

        id<MTLTexture> GetTexture() const;

        TextureView* CreateTextureView(const TextureViewCreateInfo* createInfo) override;
        void Destroy() override;
    private:
        MTLDevice &device;
        id<MTLTexture> texture;
    };
}
