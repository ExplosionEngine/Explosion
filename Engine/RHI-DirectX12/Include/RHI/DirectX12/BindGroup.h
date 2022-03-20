//
// Created by johnk on 20/3/2022.
//

#ifndef EXPLOSION_RHI_DX12_BIND_GROUP_H
#define EXPLOSION_RHI_DX12_BIND_GROUP_H

#include <vector>

#include <RHI/BindGroup.h>

namespace RHI::DirectX12 {
    class DX12BindGroup : public BindGroup {
    public:
        NON_COPYABLE(DX12BindGroup)
        DX12BindGroup(const BindGroupCreateInfo* createInfo);
        ~DX12BindGroup() override;

        void Destroy() override;

    private:
    };
}

#endif//EXPLOSION_RHI_DX12_BIND_GROUP_H
