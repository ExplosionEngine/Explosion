//
// Created by johnk on 2023/3/21.
//

#pragma once

#include <RHI/TextureView.h>

namespace RHI::Dummy {
    class DummyTextureView final : public TextureView {
    public:
        NonCopyable(DummyTextureView)
        explicit DummyTextureView(const TextureViewCreateInfo& createInfo);
        ~DummyTextureView() override;
    };
}
