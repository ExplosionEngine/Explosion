//
// Created by johnk on 22/3/2022.
//

#include <RHI/DirectX12/CommandBuffer.h>

namespace RHI::DirectX12 {
    DX12CommandBuffer::DX12CommandBuffer(DX12Device& device) {}

    DX12CommandBuffer::~DX12CommandBuffer() = default;

    CommandEncoder* DX12CommandBuffer::Begin()
    {
        return nullptr;
    }

    void DX12CommandBuffer::Destroy() {}
}
