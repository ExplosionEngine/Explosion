//
// Created by johnk on 15/1/2022.
//

#pragma once

#include <wrl/client.h>
#include <d3d12.h>

#include <RHI/Queue.h>

using Microsoft::WRL::ComPtr;

namespace RHI::DirectX12 {
    class DX12Queue : public Queue {
    public:
        NonCopyable(DX12Queue)
        explicit DX12Queue(ComPtr<ID3D12CommandQueue>&& dx12CommandQueue);
        ~DX12Queue() override;

        void Submit(CommandBuffer* commandBuffer, const QueueSubmitInfo& submitInfo) override;
        void Flush(const RHI::QueueFlushInfo& flushInfo) override;

        ComPtr<ID3D12CommandQueue>& GetDX12CommandQueue();

    private:
        ComPtr<ID3D12CommandQueue> dx12CommandQueue;
    };
}
