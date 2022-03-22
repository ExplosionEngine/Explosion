//
// Created by johnk on 22/3/2022.
//

#pragma once

#include <RHI/CommandBuffer.h>

namespace RHI::DirectX12 {
    class DX12Device;

    class DX12CommandBuffer : public CommandBuffer {
    public:
        NON_COPYABLE(DX12CommandBuffer)
        explicit DX12CommandBuffer(DX12Device& device);
        ~DX12CommandBuffer() override;

        CommandEncoder* Begin() override;
        void Destroy() override;
    };
}
