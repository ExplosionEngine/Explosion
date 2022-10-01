//
// Created by Zach Lee on 2022/10/1.
//

#pragma once

#import <Metal/Metal.h>
#include <RHI/Texture.h>

namespace RHI::Metal {
    class MTLDevice;

    class MTLTexture : public Texture {
    public:
        MTLTexture(MTLDevice &device, const TextureCreateInfo* createInfo);
        ~MTLTexture();

        TextureView* CreateTextureView(const TextureViewCreateInfo* createInfo) override;
        void Destroy() override;
    private:

    };
}