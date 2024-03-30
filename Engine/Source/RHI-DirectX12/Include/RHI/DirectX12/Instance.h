//
// Created by johnk on 10/1/2022.
//

#pragma once

#include <vector>
#include <unordered_map>
#include <functional>

#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi1_4.h>

#include <RHI/DirectX12/Api.h>
#include <RHI/Instance.h>
#include <Common/Memory.h>

using Microsoft::WRL::ComPtr;

namespace RHI::DirectX12 {
    class DX12Gpu;
    class DX12Device;

#if BUILD_CONFIG_DEBUG
    using NativeDebugLayerExceptionHandler = std::function<void()>;
#endif

    extern RHI::Instance* gInstance;

    class RHI_DIRECTX12_API DX12Instance : public Instance {
    public:
        NonCopyable(DX12Instance)
        DX12Instance();
        ~DX12Instance() noexcept override;

        RHIType GetRHIType() override;
        uint32_t GetGpuNum() override;
        Gpu* GetGpu(uint32_t index) override;
        void Destroy() override;

        IDXGIFactory4* GetNative();
#if BUILD_CONFIG_DEBUG
        void AddDebugLayerExceptionHandler(const DX12Device* inDevice, NativeDebugLayerExceptionHandler inHandler);
        void RemoveDebugLayerExceptionHandler(const DX12Device* inDevice);
        void BroadcastDebugLayerExceptions();
#endif

    private:
        void CreateNativeFactory();
        void EnumerateAdapters();
#if BUILD_CONFIG_DEBUG
        void RegisterDX12ExceptionHandler();
        void UnregisterDX12ExceptionHandler();
#endif

        ComPtr<IDXGIFactory4> nativeFactory;
        std::vector<Common::UniqueRef<DX12Gpu>> gpus;
#if BUILD_CONFIG_DEBUG
        void* nativeExceptionHandler;
        std::unordered_map<const DX12Device*, NativeDebugLayerExceptionHandler> nativeDebugLayerExceptionHandlers;
#endif
    };
}
