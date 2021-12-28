//
// Created by johnk on 27/12/2021.
//

#ifndef EXPLOSION_RHI_DX12_INSTANCE_H
#define EXPLOSION_RHI_DX12_INSTANCE_H

#include <RHI/Instance.h>
#include <RHI/DirectX12/Api.h>

extern "C" {
    RHI_DIRECTX12_API RHI::Instance* RHICreateInstance();
};

namespace RHI::DirectX12 {
    class DX12Instance : public Instance {
    public:
        DX12Instance();
        ~DX12Instance() override;
    };
}

#endif //EXPLOSION_RHI_DX12_INSTANCE_H
