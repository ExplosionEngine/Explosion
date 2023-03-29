//
// Created by Zach Lee on 2022/10/1.
//

#include <Metal/TextureView.h>
#include <Metal/Texture.h>
#include <Metal/Device.h>

namespace RHI::Metal {

    MTLTextureView::MTLTextureView(MTLTexture &texture, const TextureViewCreateInfo& createInfo)
        : TextureView(createInfo)
    {
        CreateNativeTextureView(texture, createInfo);
    }

    MTLTextureView::~MTLTextureView()
    {
    }

    void MTLTextureView::Destroy()
    {
        delete this;
    }

    id<MTLTexture> MTLTextureView::GetTexture() const
    {
        return drawable ? drawable->GetTexture() : textureView;
    }

    void MTLTextureView::CreateNativeTextureView(MTLTexture &texture, const TextureViewCreateInfo& createInfo)
    {
        if (texture.IsDrawableTexture()) {
            textureView = texture.GetNativeTexture();
        }
    }

    void MTLTextureView::SetDrawable(const DrawablePtr &value)
    {
        drawable = value;
    }

}
