//
// Created by johnk on 1/1/2022.
//

#ifndef EXPLOSION_RHI_DX12_COMMAND_QUEUE_H
#define EXPLOSION_RHI_DX12_COMMAND_QUEUE_H

#include <d3d12.h>
#include <wrl/client.h>

#include <RHI/Queue.h>
#include <Common/Utility.h>

using namespace Microsoft::WRL;

namespace RHI::DirectX12 {
    class DX12LogicalDevice;

    class DX12Queue : public Queue {
    public:
        NON_COPYABLE(DX12Queue)
        DX12Queue(DX12LogicalDevice& logicalDevice, const QueueCreateInfo* createInfo);
        ~DX12Queue() override;

        ComPtr<ID3D12CommandQueue>& GetDX12CommandQueue();

    private:
        void CreateCommandQueue(DX12LogicalDevice& logicalDevice, const QueueCreateInfo* createInfo);

        ComPtr<ID3D12CommandQueue> dx12CommandQueue;
    };
}

#endif //EXPLOSION_RHI_DX12_COMMAND_QUEUE_H
