//
// Created by johnk on 5/3/2022.
//

#pragma once

#include <memory>

#include <directx/d3dx12.h>

#include <RHI/Sampler.h>

namespace RHI::DirectX12 {
    class DX12Device;

    class DX12Sampler : public Sampler {
    public:
        NonCopyable(DX12Sampler)
        explicit DX12Sampler(DX12Device& inDevice, const SamplerCreateInfo& inCreateInfo);
        ~DX12Sampler() override;

        void Destroy() override;

        CD3DX12_CPU_DESCRIPTOR_HANDLE GetNativeCpuDescriptorHandle();

    private:
        void CreateDX12Descriptor(DX12Device& inDevice, const SamplerCreateInfo& inCreateInfo);

        CD3DX12_CPU_DESCRIPTOR_HANDLE nativeCpuDescriptorHandle;
    };
}
