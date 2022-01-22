//
// Created by johnk on 15/1/2022.
//

#include <RHI/DirectX12/Queue.h>

namespace RHI::DirectX12 {
    DX12Queue::DX12Queue(ComPtr<ID3D12CommandQueue>&& q) : Queue(), dx12CommandQueue(q) {}

    DX12Queue::~DX12Queue() = default;

    ComPtr<ID3D12CommandQueue>& DX12Queue::GetDX12CommandQueue()
    {
        return dx12CommandQueue;
    }
}
