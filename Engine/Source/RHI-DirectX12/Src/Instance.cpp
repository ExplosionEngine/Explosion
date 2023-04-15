//
// Created by johnk on 10/1/2022.
//

#include <utility>

#include <RHI/DirectX12/Common.h>
#include <RHI/DirectX12/Instance.h>
#include <RHI/DirectX12/Gpu.h>

namespace RHI::DirectX12 {
#if BUILD_CONFIG_DEBUG
    static LONG __stdcall DX12VectoredExceptionHandler(EXCEPTION_POINTERS* info)
    {
        if (info->ExceptionRecord->ExceptionCode != _FACDXGI)
        {
            return EXCEPTION_CONTINUE_SEARCH;
        }

        dynamic_cast<DX12Instance*>(RHIGetInstance())->BroadcastDebugLayerExceptions();
        return EXCEPTION_CONTINUE_EXECUTION;
    }
#endif
}

namespace RHI::DirectX12 {
    DX12Instance::DX12Instance() : Instance()
    {
        CreateDX12Factory();
        EnumerateAdapters();
#if BUILD_CONFIG_DEBUG
        RegisterDX12ExceptionHandler();
#endif
    }

    DX12Instance::~DX12Instance()
    {
#if BUILD_CONFIG_DEBUG
        UnregisterDX12ExceptionHandler();
#endif
    }

    RHIType DX12Instance::GetRHIType()
    {
        return RHIType::DIRECTX_12;
    }

    ComPtr<IDXGIFactory4>& DX12Instance::GetDX12Factory()
    {
        return dx12Factory;
    }

#if BUILD_CONFIG_DEBUG
    void DX12Instance::RegisterDX12ExceptionHandler()
    {
        dx12ExceptionHandler = AddVectoredExceptionHandler(1, DX12VectoredExceptionHandler);
    }

    void DX12Instance::UnregisterDX12ExceptionHandler()
    {
        RemoveVectoredExceptionHandler(dx12ExceptionHandler);
    }

    void DX12Instance::AddDebugLayerExceptionHandler(const DX12Device* device, DebugLayerExceptionHandler handler)
    {
        auto iter = debugLayerExceptionHandlers.find(device);
        Assert(iter == debugLayerExceptionHandlers.end());
        debugLayerExceptionHandlers[device] = std::move(handler);
    }

    void DX12Instance::RemoveDebugLayerExceptionHandler(const DX12Device* device)
    {
        auto iter = debugLayerExceptionHandlers.find(device);
        Assert(iter != debugLayerExceptionHandlers.end());
        debugLayerExceptionHandlers.erase(iter);
    }

    void DX12Instance::BroadcastDebugLayerExceptions()
    {
        for (const auto& handler : debugLayerExceptionHandlers)
        {
            handler.second();
        }
    }
#endif

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

        bool success = SUCCEEDED(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&dx12Factory)));
        Assert(success);
    }

    void DX12Instance::EnumerateAdapters()
    {
        ComPtr<IDXGIAdapter1> tempAdapter;
        for (uint32_t i = 0; SUCCEEDED(dx12Factory->EnumAdapters1(i, &tempAdapter)); i++) {
            gpus.emplace_back(std::make_unique<DX12Gpu>(*this, std::move(tempAdapter)));
            tempAdapter = nullptr;
        }
    }

    uint32_t DX12Instance::GetGpuNum()
    {
        return gpus.size();
    }

    Gpu* DX12Instance::GetGpu(uint32_t index)
    {
        return gpus[index].Get();
    }

    void DX12Instance::Destroy()
    {
        delete this;
    }
}

extern "C" {
    RHI::Instance* RHIGetInstance()
    {
        static RHI::DirectX12::DX12Instance singleton;
        return &singleton;
    }
}
