//
// Created by johnk on 2023/8/7.
//

#include <RHI/DirectX12/DX12RHIModule.h>
#include <RHI/DirectX12/Instance.h>

namespace RHI::DirectX12 {
    DX12RHIModule::DX12RHIModule() = default;

    DX12RHIModule::~DX12RHIModule() = default;

    Instance* DX12RHIModule::GetRHIInstance() // NOLINT
    {
        return RHIGetInstance();
    }
}

IMPLEMENT_MODULE(RHI_DIRECTX12_API, RHI::DirectX12::DX12RHIModule);
