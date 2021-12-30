//
// Created by johnk on 27/12/2021.
//

#ifndef EXPLOSION_RHI_DX12_INSTANCE_H
#define EXPLOSION_RHI_DX12_INSTANCE_H

#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>

#include <RHI/Instance.h>
#include <RHI/DirectX12/Api.h>

using namespace Microsoft::WRL;

extern "C" {
    RHI_DIRECTX12_API RHI::Instance* RHICreateInstance(const RHI::InstanceCreateInfo& info);
};

namespace RHI::DirectX12 {
    class DX12Instance : public Instance {
    public:
        explicit DX12Instance(const InstanceCreateInfo& info);
        ~DX12Instance() override;

    private:
        void CreateFactory(const InstanceCreateInfo& info);

        ComPtr<IDXGIFactory4> dxgiFactory;
    };
}

#endif //EXPLOSION_RHI_DX12_INSTANCE_H
