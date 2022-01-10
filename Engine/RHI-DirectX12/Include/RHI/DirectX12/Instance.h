//
// Created by johnk on 10/1/2022.
//

#ifndef EXPLOSION_RHI_DX12_INSTANCE_H
#define EXPLOSION_RHI_DX12_INSTANCE_H

#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi1_4.h>

#include <RHI/DirectX12/Api.h>
#include <RHI/Instance.h>

using Microsoft::WRL::ComPtr;

namespace RHI::DirectX12 {
    class RHI_DIRECTX12_API DX12Instance : public Instance {
    public:
        DX12Instance();
        ~DX12Instance() noexcept override;
        RHIType GetRHIType() override;

    private:
        void CreateDX12Factory();

        ComPtr<IDXGIFactory4> dx12Factory;
    };

    RHI_DIRECTX12_API Instance* RHICreateInstance();
}

#endif //EXPLOSION_RHI_DX12_INSTANCE_H
