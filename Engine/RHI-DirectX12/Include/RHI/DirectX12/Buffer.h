//
// Created by johnk on 2022/1/24.
//

#ifndef EXPLOSION_RHI_DX12_BUFFER_H
#define EXPLOSION_RHI_DX12_BUFFER_H

#include <wrl/client.h>
#include <d3d12.h>

#include <RHI/Buffer.h>

using namespace Microsoft::WRL;

namespace RHI::DirectX12 {
    class DX12Device;

    class DX12Buffer : public Buffer {
    public:
        explicit DX12Buffer(DX12Device& device, const BufferCreateInfo* createInfo);
        ~DX12Buffer() override;

        void* Map(MapMode mapMode, size_t offset, size_t length) override;
        void UnMap() override;
        void Destroy() override;

        ComPtr<ID3D12Resource>& GetDX12Resource();

    private:
        void CreateBuffer(DX12Device& device, const BufferCreateInfo* createInfo);

        ComPtr<ID3D12Resource> dx12Resource;
    };
}

#endif //EXPLOSION_RHI_DX12_BUFFER_H
