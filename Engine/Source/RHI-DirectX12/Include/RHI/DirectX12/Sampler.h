//
// Created by johnk on 5/3/2022.
//

#pragma once


#include <directx/d3dx12.h>

#include <RHI/Sampler.h>
#include <RHI/DirectX12/Device.h>

namespace RHI::DirectX12 {
    class DX12Device;

    class DX12Sampler final : public Sampler {
    public:
        NonCopyable(DX12Sampler)
        explicit DX12Sampler(DX12Device& inDevice, const SamplerCreateInfo& inCreateInfo);
        ~DX12Sampler() override;

        CD3DX12_CPU_DESCRIPTOR_HANDLE GetNativeCpuDescriptorHandle() const;

    private:
        void CreateDX12Descriptor(DX12Device& inDevice, const SamplerCreateInfo& inCreateInfo);

        Common::UniqueRef<DescriptorAllocation> descriptorAllocation;
    };
}
