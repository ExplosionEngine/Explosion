//
// Created by johnk on 1/1/2022.
//

#include <RHI/DirectX12/Queue.h>
#include <RHI/DirectX12/Enum.h>
#include <RHI/DirectX12/LogicalDevice.h>

namespace RHI::DirectX12 {
    DX12Queue::DX12Queue(DX12LogicalDevice& logicalDevice, const QueueCreateInfo* createInfo) : Queue(createInfo)
    {
        CreateCommandQueue(logicalDevice, createInfo);
    }

    DX12Queue::~DX12Queue() = default;

    ComPtr<ID3D12CommandQueue>& DX12Queue::GetDX12CommandQueue()
    {
        return dx12CommandQueue;
    }

    void DX12Queue::CreateCommandQueue(DX12LogicalDevice& logicalDevice, const QueueCreateInfo* createInfo)
    {
        D3D12_COMMAND_QUEUE_DESC desc {};
        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.Type = EnumCast<QueueFamilyType, D3D12_COMMAND_LIST_TYPE>(createInfo->type);

        ThrowIfFailed(
            logicalDevice.GetDX12Device()->CreateCommandQueue(&desc, IID_PPV_ARGS(&dx12CommandQueue)),
            "failed to create dx12 command queue"
        );
    }
}
