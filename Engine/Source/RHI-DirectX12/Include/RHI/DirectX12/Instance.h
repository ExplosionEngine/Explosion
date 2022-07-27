//
// Created by johnk on 10/1/2022.
//

#pragma once

#include <vector>
#include <memory>

#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi1_4.h>

#include <RHI/DirectX12/Api.h>
#include <RHI/Instance.h>

using Microsoft::WRL::ComPtr;

namespace RHI::DirectX12 {
    class DX12Gpu;

    class RHI_DIRECTX12_API DX12Instance : public Instance {
    public:
        NON_COPYABLE(DX12Instance)
        DX12Instance();
        ~DX12Instance() noexcept override;

        RHIType GetRHIType() override;
        uint32_t GetGpuNum() override;
        Gpu* GetGpu(uint32_t index) override;
        void Destroy() override;

        ComPtr<IDXGIFactory4>& GetDX12Factory();

    private:
        void CreateDX12Factory();
        void EnumerateAdapters();

        std::vector<std::unique_ptr<DX12Gpu>> gpus;
        ComPtr<IDXGIFactory4> dx12Factory;
    };
}

extern "C" {
    RHI_DIRECTX12_API RHI::Instance* RHIGetInstance();
}
