//
// Created by johnk on 27/12/2021.
//

#include <RHI/DirectX12/Instance.h>

extern "C" {
    RHI::Instance* RHICreateInstance()
    {
        return new RHI::DirectX12::DX12Instance;
    }
}

namespace RHI::DirectX12 {
    DX12Instance::DX12Instance() = default;

    DX12Instance::~DX12Instance() = default;
}
