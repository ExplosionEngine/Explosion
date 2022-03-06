//
// Created by johnk on 5/3/2022.
//

#ifndef EXPLOSION_RHI_DX12_SAMPLER_H
#define EXPLOSION_RHI_DX12_SAMPLER_H

#include <memory>

#include <RHI/Sampler.h>
#include <d3d12.h>

namespace RHI::DirectX12 {
    class DX12Sampler : public Sampler {
    public:
        NON_COPYABLE(DX12Sampler)
        DX12Sampler(const SamplerCreateInfo* createInfo);
        ~DX12Sampler();

        void Destroy() override;

        D3D12_STATIC_SAMPLER_DESC* GetDesc() const;

    private:
        void CreateDesc(const SamplerCreateInfo* createInfo);

        std::unique_ptr<D3D12_STATIC_SAMPLER_DESC> desc;
    };
}

#endif//EXPLOSION_RHI_DX12_SAMPLER_H