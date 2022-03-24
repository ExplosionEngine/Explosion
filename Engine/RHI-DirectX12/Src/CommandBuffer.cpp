//
// Created by johnk on 22/3/2022.
//

#include <RHI/DirectX12/CommandBuffer.h>
#include <RHI/DirectX12/Common.h>
#include <RHI/DirectX12/Device.h>
#include <RHI/DirectX12/CommandEncoder.h>

namespace RHI::DirectX12 {
    DX12CommandBuffer::DX12CommandBuffer(DX12Device& device) : CommandBuffer()
    {
        AllocateDX12CommandList(device);
    }

    DX12CommandBuffer::~DX12CommandBuffer() = default;

    CommandEncoder* DX12CommandBuffer::Begin()
    {
        return new DX12CommandEncoder(*this);
    }

    void DX12CommandBuffer::Destroy()
    {
        delete this;
    }

    ComPtr<ID3D12GraphicsCommandList>& DX12CommandBuffer::GetDX12GraphicsCommandList()
    {
        return dx12GraphicsCommandList;
    }

    void DX12CommandBuffer::AllocateDX12CommandList(DX12Device& device)
    {
        if (FAILED(device.GetDX12Device()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, device.GetDX12CommandAllocator().Get(), nullptr, IID_PPV_ARGS(&dx12GraphicsCommandList)))) {
            throw DX12Exception("failed to allocate dx12 command list");
        }
    }
}
