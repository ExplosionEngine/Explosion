//
// Created by Zach Lee on 2022/10/1.
//

#include <Metal/Texture.h>
#include <Metal/Device.h>

namespace RHI::Metal {

    MTLTexture::MTLTexture(MTLDevice &dev, const TextureCreateInfo* createInfo)
        : Texture(createInfo), device(dev), isDrawable(createInfo == nullptr)
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

    id<MTLTexture> MTLTexture::GetNativeTexture() const
    {
        return texture;
    }

    bool MTLTexture::IsDrawableTexture() const
    {
        return isDrawable;
    }
}
