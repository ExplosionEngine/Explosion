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

        static_cast<DX12Instance*>(gInstance)->BroadcastDebugLayerExceptions();
        return EXCEPTION_CONTINUE_EXECUTION;
    }
#endif
}

namespace RHI::DirectX12 {
    RHI::Instance* gInstance = nullptr;

    DX12Instance::DX12Instance() : Instance()
    {
        CreateNativeFactory();
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
        return RHIType::directX12;
    }

    IDXGIFactory4* DX12Instance::GetNative()
    {
        return nativeFactory.Get();
    }

#if BUILD_CONFIG_DEBUG
    void DX12Instance::RegisterDX12ExceptionHandler()
    {
        nativeExceptionHandler = AddVectoredExceptionHandler(1, DX12VectoredExceptionHandler);
    }

    void DX12Instance::UnregisterDX12ExceptionHandler()
    {
        RemoveVectoredExceptionHandler(nativeExceptionHandler);
    }

    void DX12Instance::AddDebugLayerExceptionHandler(const DX12Device* inDevice, NativeDebugLayerExceptionHandler inHandler)
    {
        auto iter = nativeDebugLayerExceptionHandlers.find(inDevice);
        Assert(iter == nativeDebugLayerExceptionHandlers.end());
        nativeDebugLayerExceptionHandlers[inDevice] = std::move(inHandler);
    }

    void DX12Instance::RemoveDebugLayerExceptionHandler(const DX12Device* inDevice)
    {
        auto iter = nativeDebugLayerExceptionHandlers.find(inDevice);
        Assert(iter != nativeDebugLayerExceptionHandlers.end());
        nativeDebugLayerExceptionHandlers.erase(iter);
    }

    void DX12Instance::BroadcastDebugLayerExceptions()
    {
        for (const auto& handler : nativeDebugLayerExceptionHandlers)
        {
            handler.second();
        }
    }
#endif

    void DX12Instance::CreateNativeFactory()
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

        bool success = SUCCEEDED(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&nativeFactory)));
        Assert(success);
    }

    void DX12Instance::EnumerateAdapters()
    {
        ComPtr<IDXGIAdapter1> tempAdapter;
        for (uint32_t i = 0; SUCCEEDED(nativeFactory->EnumAdapters1(i, &tempAdapter)); i++) {
            gpus.emplace_back(Common::MakeUnique<DX12Gpu>(*this, std::move(tempAdapter)));
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
