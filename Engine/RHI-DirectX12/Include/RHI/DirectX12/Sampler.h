//
// Created by johnk on 5/3/2022.
//

#ifndef EXPLOSION_RHI_DX12_SAMPLER_H
#define EXPLOSION_RHI_DX12_SAMPLER_H

#include <memory>

#include <directx/d3dx12.h>

#include <RHI/Sampler.h>

namespace RHI::DirectX12 {
    class DX12Device;

    class DX12Sampler : public Sampler {
    public:
        NON_COPYABLE(DX12Sampler)
        explicit DX12Sampler(DX12Device& device, const SamplerCreateInfo* createInfo);
        ~DX12Sampler() override;

        void Destroy() override;

        CD3DX12_CPU_DESCRIPTOR_HANDLE GetDX12CpuDescriptorHandle();

    private:
        void CreateDX12Descriptor(DX12Device& device, const SamplerCreateInfo* createInfo);

        CD3DX12_CPU_DESCRIPTOR_HANDLE dx12CpuDescriptorHandle;
    };
}

#endif//EXPLOSION_RHI_DX12_SAMPLER_H
