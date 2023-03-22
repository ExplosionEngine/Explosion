//
// Created by johnk on 2023/3/21.
//

#include <RHI/Dummy/TextureView.h>

namespace RHI::Dummy {
    DummyTextureView::DummyTextureView(const TextureViewCreateInfo* createInfo)
        : TextureView(createInfo)
    {
    }

    DummyTextureView::~DummyTextureView() = default;

    void DummyTextureView::Destroy()
    {
        delete this;
    }
}
