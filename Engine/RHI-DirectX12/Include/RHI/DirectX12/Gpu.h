//
// Created by johnk on 13/1/2022.
//

#ifndef EXPLOSION_RHI_DX12_GPU_H
#define EXPLOSION_RHI_DX12_GPU_H

#include <wrl/client.h>
#include <dxgi1_4.h>

#include <RHI/Gpu.h>

using namespace Microsoft::WRL;

namespace RHI::DirectX12 {
    class DX12Gpu : public Gpu {
    public:
        NON_COPYABLE(DX12Gpu)
        explicit DX12Gpu(ComPtr<IDXGIAdapter1>&& adapter);
        ~DX12Gpu() override;

        GpuProperty GetProperty() override;
        ComPtr<IDXGIAdapter1>& GetDX12Adapter();

    private:
        ComPtr<IDXGIAdapter1> dx12Adapter;
    };
}

#endif //EXPLOSION_RHI_DX12_GPU_H
