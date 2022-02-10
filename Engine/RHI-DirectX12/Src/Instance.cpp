//
// Created by johnk on 10/1/2022.
//

#include <RHI/DirectX12/Instance.h>
#include <RHI/DirectX12/Gpu.h>

namespace RHI::DirectX12 {
    DX12Instance::DX12Instance() : Instance()
    {
        CreateDX12Factory();
        EnumerateAdapters();
    }

    DX12Instance::~DX12Instance() noexcept = default;

    RHIType DX12Instance::GetRHIType()
    {
        return RHIType::DIRECTX_12;
    }

    void DX12Instance::CreateDX12Factory()
    {
        UINT factoryFlags = 0;

#if BUILD_CONFIG_DEBUG
        {
            ComPtr<ID3D12Debug> debugController;
            if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
                debugController->EnableDebugLayer();
                factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
            }
        }
#endif

        if (FAILED(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&dx12Factory)))) {
            throw DX12Exception("failed to create dxgi factory");
        }
    }

    void DX12Instance::EnumerateAdapters()
    {
        ComPtr<IDXGIAdapter1> tempAdapter;
        for (uint32_t i = 0; SUCCEEDED(dx12Factory->EnumAdapters1(i, &tempAdapter)); i++) {
            gpus.emplace_back(std::make_unique<DX12Gpu>(std::move(tempAdapter)));
            tempAdapter = nullptr;
        }
    }

    uint32_t DX12Instance::GetGpuNum()
    {
        return gpus.size();
    }

    Gpu* DX12Instance::GetGpu(uint32_t index)
    {
        return gpus[index].get();
    }
}

extern "C" {
    RHI::Instance* RHICreateInstance()
    {
        static RHI::DirectX12::DX12Instance singleton;
        return &singleton;
    }
}
