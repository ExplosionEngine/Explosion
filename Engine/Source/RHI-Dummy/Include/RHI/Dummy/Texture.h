//
// Created by johnk on 2023/3/21.
//

#pragma once

#include <RHI/Texture.h>

namespace RHI::Dummy {
    class DummyTexture final : public Texture {
    public:
        NonCopyable(DummyTexture)
        explicit DummyTexture(const TextureCreateInfo& createInfo);
        ~DummyTexture() override;

        Common::UniquePtr<TextureView> CreateTextureView(const TextureViewCreateInfo& createInfo) override;
    };
}
