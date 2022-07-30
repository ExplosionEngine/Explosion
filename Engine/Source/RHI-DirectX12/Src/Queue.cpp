//
// Created by johnk on 15/1/2022.
//

#include <array>

#include <Common/Debug.h>
#include <RHI/DirectX12/Queue.h>
#include <RHI/DirectX12/CommandBuffer.h>
#include <RHI/DirectX12/Synchronous.h>

namespace RHI::DirectX12 {
    DX12Queue::DX12Queue(ComPtr<ID3D12CommandQueue>&& q) : Queue(), dx12CommandQueue(q) {}

    DX12Queue::~DX12Queue() = default;

    void DX12Queue::Submit(CommandBuffer* inCommandBuffer, Fence* inFenceToSignal)
    {
        auto* commandBuffer = dynamic_cast<DX12CommandBuffer*>(inCommandBuffer);
        auto* fenceToSignal = dynamic_cast<DX12Fence*>(inFenceToSignal);
        Assert(commandBuffer);

        std::array<ID3D12CommandList*, 1> cmdListsToExecute = { commandBuffer->GetDX12GraphicsCommandList().Get() };
        dx12CommandQueue->ExecuteCommandLists(cmdListsToExecute.size(), cmdListsToExecute.data());

        if (fenceToSignal != nullptr) {
            fenceToSignal->Reset();
            dx12CommandQueue->Signal(fenceToSignal->GetDX12Fence().Get(), 1);
        }
    }

    void DX12Queue::Wait(Fence* inFenceToSignal)
    {
        auto* fenceToSignal = dynamic_cast<DX12Fence*>(inFenceToSignal);
        if (fenceToSignal != nullptr) {
            fenceToSignal->Reset();
            dx12CommandQueue->Signal(fenceToSignal->GetDX12Fence().Get(), 1);
        }
    }

    ComPtr<ID3D12CommandQueue>& DX12Queue::GetDX12CommandQueue()
    {
        return dx12CommandQueue;
    }
}
