//
// Created by johnk on 2023/3/21.
//

#include <RHI/Dummy/Texture.h>
#include <RHI/Dummy/TextureView.h>

namespace RHI::Dummy {
    DummyTexture::DummyTexture(const TextureCreateInfo& createInfo)
        : Texture(createInfo)
    {
    }

    DummyTexture::~DummyTexture() = default;

    TextureView* DummyTexture::CreateTextureView(const TextureViewCreateInfo& createInfo)
    {
        return new DummyTextureView(createInfo);
    }

    void DummyTexture::Destroy()
    {
        delete this;
    }
}
