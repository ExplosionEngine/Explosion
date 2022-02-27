//
// Created by johnk on 2022/2/25.
//

#include <RHI/DirectX12/TextureView.h>

namespace RHI::DirectX12 {
    DX12TextureView::DX12TextureView(DX12Texture* texture, const TextureViewCreateInfo* createInfo) : TextureView(createInfo), texture(texture)
    {
        CreateDesc();
    }

    DX12TextureView::~DX12TextureView() = default;

    void DX12TextureView::Destroy()
    {
        delete this;
    }

    void DX12TextureView::CreateDesc()
    {
        // TODO
    }
}
