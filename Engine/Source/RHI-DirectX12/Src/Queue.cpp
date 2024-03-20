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
        auto* waitFence = static_cast<DX12Fence*>(submitInfo.waitFence);
        auto* signalFence = static_cast<DX12Fence*>(submitInfo.signalFence);
        Assert(commandBuffer);

        if (waitFence != nullptr) {
            dx12CommandQueue->Wait(waitFence->GetDX12Fence().Get(), 1);
        }

        std::array<ID3D12CommandList*, 1> cmdListsToExecute = { commandBuffer->GetDX12GraphicsCommandList().Get() };
        dx12CommandQueue->ExecuteCommandLists(cmdListsToExecute.size(), cmdListsToExecute.data());

        QueueFlushInfo flushInfo {};
        flushInfo.signalFence = signalFence;
        flushInfo.signalFenceValue = submitInfo.signalFenceValue;

        Flush(flushInfo);
    }

    void DX12Queue::Flush(const QueueFlushInfo& flushInfo)
    {
        auto* signalFence = static_cast<DX12Fence*>(flushInfo.signalFence);

        if (signalFence != nullptr) {
            dx12CommandQueue->Signal(signalFence->GetDX12Fence().Get(), flushInfo.signalFenceValue);
        }
    }

    ComPtr<ID3D12CommandQueue>& DX12Queue::GetDX12CommandQueue()
    {
        return dx12CommandQueue;
    }
}
