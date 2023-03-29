//
// Created by Zach Lee on 2022/10/1.
//

#include <Metal/Texture.h>
#include <Metal/Device.h>

namespace RHI::Metal {

    MTLTexture::MTLTexture(MTLDevice& dev, const TextureCreateInfo& createInfo)
        : Texture(createInfo), device(dev)
    {
        // TODO Create Native Texture

    }

    MTLTexture::MTLTexture(MTLDevice& device)
        : device(device)
    {
        drawable = std::make_shared<DrawableProvider>();
    }

    MTLTexture::~MTLTexture()
    {
    }

    TextureView* MTLTexture::CreateTextureView(const TextureViewCreateInfo& createInfo)
    {
        auto textureView = new MTLTextureView(*this, createInfo);
        textureView->SetDrawable(drawable);
        return textureView;
    }

    void MTLTexture::Destroy()
    {
        delete this;
    }

    id<MTLTexture> MTLTexture::GetNativeTexture() const
    {
        return drawable ? drawable->GetTexture() : texture;
    }

    void MTLTexture::SetDrawable(id<CAMetalDrawable> value)
    {
        drawable->SetDrawable(value);
    }

    void MTLTexture::ResetDrawable()
    {
        drawable->Reset();
    }

    bool MTLTexture::IsDrawableTexture() const
    {
        return !!drawable;
    }
}
