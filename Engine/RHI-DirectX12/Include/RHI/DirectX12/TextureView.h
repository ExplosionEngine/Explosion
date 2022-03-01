//
// Created by johnk on 2022/2/25.
//

#ifndef EXPLOSION_RHI_DX12_TEXTURE_VIEW_H
#define EXPLOSION_RHI_DX12_TEXTURE_VIEW_H

#include <memory>

#include <wrl/client.h>
#include <d3d12.h>
using namespace Microsoft::WRL;

#include <RHI/TextureView.h>

namespace RHI::DirectX12 {
    class DX12Device;
    class DX12Texture;

    class DX12TextureView : public TextureView {
    public:
        NON_COPYABLE(DX12TextureView)
        explicit DX12TextureView(DX12Texture& texture, const TextureViewCreateInfo* createInfo);
        ~DX12TextureView() override;

        void Destroy() override;

    private:
        void CreateDesc(const TextureViewCreateInfo* createInfo);

        DX12Texture& texture;
        std::unique_ptr<D3D12_SHADER_RESOURCE_VIEW_DESC> srvDesc;
        std::unique_ptr<D3D12_UNORDERED_ACCESS_VIEW_DESC> uavDesc;
        std::unique_ptr<D3D12_RENDER_TARGET_VIEW_DESC> rtvDesc;
    };
}

#endif //EXPLOSION_RHI_DX12_TEXTURE_VIEW_H
