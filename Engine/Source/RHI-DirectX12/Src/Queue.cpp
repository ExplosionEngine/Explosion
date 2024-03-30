//
// Created by johnk on 15/1/2022.
//

#include <array>

#include <Common/Debug.h>
#include <RHI/DirectX12/Queue.h>
#include <RHI/DirectX12/CommandBuffer.h>
#include <RHI/DirectX12/Synchronous.h>

namespace RHI::DirectX12 {
    DX12Queue::DX12Queue(ComPtr<ID3D12CommandQueue>&& inNativeCmdQueue) : Queue(), nativeCmdQueue(inNativeCmdQueue) {}

    DX12Queue::~DX12Queue() = default;

    void DX12Queue::Submit(CommandBuffer* inCmdBuffer, const QueueSubmitInfo& inSubmitInfo)
    {
        auto* commandBuffer = static_cast<DX12CommandBuffer*>(inCmdBuffer);
        Assert(commandBuffer);

        for (auto i = 0; i < inSubmitInfo.waitSemaphores.size(); i++) {
            auto* waitSemaphore = static_cast<DX12Semaphore*>(inSubmitInfo.waitSemaphores[i]);
            nativeCmdQueue->Wait(waitSemaphore->GetNative(), 1);
        }

        std::array<ID3D12CommandList*, 1> cmdListsToExecute = { commandBuffer->GetNative() };
        nativeCmdQueue->ExecuteCommandLists(cmdListsToExecute.size(), cmdListsToExecute.data());

        for (auto i = 0; i < inSubmitInfo.signalSemaphores.size(); i++) {
            auto* signalSemaphore = static_cast<DX12Semaphore*>(inSubmitInfo.signalSemaphores[i]);
            auto* nativeFence = signalSemaphore->GetNative();
            nativeFence->Signal(0);
            nativeCmdQueue->Signal(nativeFence, 1);
        }
        Flush(inSubmitInfo.signalFence);
    }

    void DX12Queue::Flush(Fence* inFenceToSignal)
    {
        auto* nativeFence = static_cast<DX12Fence*>(inFenceToSignal);

        if (nativeFence != nullptr) {
            nativeCmdQueue->Signal(nativeFence->GetNative(), 1);
        }
    }

    ID3D12CommandQueue* DX12Queue::GetNative()
    {
        return nativeCmdQueue.Get();
    }
}
