//
// Created by Zach Lee on 2022/10/1.
//

#include <Metal/Texture.h>
#include <Metal/Device.h>

namespace RHI::Metal {

    MTLTexture::MTLTexture(MTLDevice &dev, const TextureCreateInfo* createInfo)
        : Texture(createInfo), device(dev)
    {
    }

    MTLTexture::~MTLTexture()
    {
    }

    TextureView* MTLTexture::CreateTextureView(const TextureViewCreateInfo* createInfo)
    {
    }

    void MTLTexture::Destroy()
    {
        delete this;
    }

    id<MTLTexture> MTLTexture::GetTexture() const
    {
        return texture;
    }
}
