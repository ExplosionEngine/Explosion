//
// Created by johnk on 10/1/2022.
//

#include <Common/Logger.h>
#include <RHI/DirectX12/Common.h>
#include <RHI/DirectX12/Instance.h>

namespace RHI::DirectX12 {
    static auto& logger = Common::Logger::Singleton().FindOrCreateDelegator("RHI-DirectX12");

    DX12Instance::DX12Instance() : Instance()
    {
        CreateDX12Factory();
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

    Instance* RHICreateInstance()
    {
        static DX12Instance singleton;
        return &singleton;
    }
}
