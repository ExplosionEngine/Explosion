//
// Created by johnk on 2023/8/7.
//

#pragma once

#include <RHI/RHIModule.h>
#include <RHI/DirectX12/Api.h>

namespace RHI::DirectX12 {
    class RHI_DIRECTX12_API DX12RHIModule : public RHIModule {
    public:
        DX12RHIModule();
        ~DX12RHIModule() override;

        void OnLoad() override;
        void OnUnload() override;
        Instance* GetRHIInstance() override;
    };
}
