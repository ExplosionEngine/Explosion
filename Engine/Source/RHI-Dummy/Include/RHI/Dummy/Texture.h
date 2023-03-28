//
// Created by johnk on 2023/3/21.
//

#pragma once

#include <RHI/Texture.h>

namespace RHI::Dummy {
    class DummyTexture : public Texture {
    public:
        explicit DummyTexture(const TextureCreateInfo& createInfo);
        DummyTexture() {}
        ~DummyTexture() override;

        TextureView* CreateTextureView(const TextureViewCreateInfo& createInfo) override;
        void Destroy() override;
    };
}
