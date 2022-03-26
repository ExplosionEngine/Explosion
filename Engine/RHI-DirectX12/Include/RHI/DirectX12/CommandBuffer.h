//
// Created by johnk on 22/3/2022.
//

#pragma once

#include <wrl/client.h>
#include <d3d12.h>
using namespace Microsoft::WRL;

#include <RHI/CommandBuffer.h>

namespace RHI::DirectX12 {
    class DX12Device;

    class DX12CommandBuffer : public CommandBuffer {
    public:
        NON_COPYABLE(DX12CommandBuffer)
        explicit DX12CommandBuffer(DX12Device& device);
        ~DX12CommandBuffer() override;

        CommandEncoder* Begin() override;
        void Destroy() override;

        ComPtr<ID3D12GraphicsCommandList>& GetDX12GraphicsCommandList();

    private:
        void AllocateDX12CommandList(DX12Device& device);

        DX12Device& device;
        ComPtr<ID3D12GraphicsCommandList> dx12GraphicsCommandList;
    };
}
