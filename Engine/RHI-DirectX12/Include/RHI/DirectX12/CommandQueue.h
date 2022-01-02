//
// Created by johnk on 1/1/2022.
//

#ifndef EXPLOSION_RHI_DX12_COMMAND_QUEUE_H
#define EXPLOSION_RHI_DX12_COMMAND_QUEUE_H

#include <RHI/CommandQueue.h>
#include <RHI/DirectX12/LogicalDevice.h>
#include <Common/Utility.h>

namespace RHI::DirectX12 {
    class DX12CommandQueue : public CommandQueue {
    public:
        NON_COPYABLE(DX12CommandQueue)
        DX12CommandQueue(DX12LogicalDevice& logicalDevice, const CommandQueueCreateInfo* createInfo);
        ~DX12CommandQueue() override;

        ComPtr<ID3D12CommandQueue>& GetDX12CommandQueue();

    private:
        void CreateCommandQueue(DX12LogicalDevice& logicalDevice, const CommandQueueCreateInfo* createInfo);

        ComPtr<ID3D12CommandQueue> dx12CommandQueue;
    };
}

#endif //EXPLOSION_RHI_DX12_COMMAND_QUEUE_H
