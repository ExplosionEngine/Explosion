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

    void DX12Queue::Submit(CommandBuffer* inCommandBuffer, const QueueSubmitInfo& submitInfo)
    {
        auto* commandBuffer = static_cast<DX12CommandBuffer*>(inCommandBuffer);
        Assert(commandBuffer);

        for (auto i = 0; i < submitInfo.waitSemaphores.size(); i++) {
            auto* waitSemaphore = static_cast<DX12Semaphore*>(submitInfo.waitSemaphores[i]);
            auto& dx12Fence = waitSemaphore->GetDX12Fence();
            dx12CommandQueue->Wait(dx12Fence.Get(), 1);
        }

        std::array<ID3D12CommandList*, 1> cmdListsToExecute = { commandBuffer->GetDX12GraphicsCommandList().Get() };
        dx12CommandQueue->ExecuteCommandLists(cmdListsToExecute.size(), cmdListsToExecute.data());

        for (auto i = 0; i < submitInfo.signalSemaphores.size(); i++) {
            auto* signalSemaphore = static_cast<DX12Semaphore*>(submitInfo.signalSemaphores[i]);
            auto& dx12fence = signalSemaphore->GetDX12Fence();
            dx12fence->Signal(0);
            dx12CommandQueue->Signal(dx12fence.Get(), 1);
        }
        Flush(submitInfo.signalFence);
    }

    void DX12Queue::Flush(Fence* fenceToSignal)
    {
        auto* dx12Fence = static_cast<DX12Fence*>(fenceToSignal);

        if (dx12Fence != nullptr) {
            dx12CommandQueue->Signal(dx12Fence->GetDX12Fence().Get(), 1);
        }
    }

    ComPtr<ID3D12CommandQueue>& DX12Queue::GetDX12CommandQueue()
    {
        return dx12CommandQueue;
    }
}
