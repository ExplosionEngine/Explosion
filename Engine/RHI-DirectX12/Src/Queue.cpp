//
// Created by johnk on 15/1/2022.
//

#include <array>

#include <Common/Debug.h>
#include <RHI/DirectX12/Queue.h>
#include <RHI/DirectX12/CommandBuffer.h>

namespace RHI::DirectX12 {
    DX12Queue::DX12Queue(ComPtr<ID3D12CommandQueue>&& q) : Queue(), dx12CommandQueue(q) {}

    DX12Queue::~DX12Queue() = default;

    void DX12Queue::Submit(CommandBuffer* cb)
    {
        auto* commandBuffer = dynamic_cast<DX12CommandBuffer*>(cb);
        Assert(commandBuffer);
        std::array<ID3D12CommandList*, 1> cmdListsToExecute = { commandBuffer->GetDX12GraphicsCommandList().Get() };
        dx12CommandQueue->ExecuteCommandLists(cmdListsToExecute.size(), cmdListsToExecute.data());
    }

    ComPtr<ID3D12CommandQueue>& DX12Queue::GetDX12CommandQueue()
    {
        return dx12CommandQueue;
    }
}
