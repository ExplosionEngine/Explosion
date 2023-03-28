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
        NON_COPYABLE(DX12Sampler)
        explicit DX12Sampler(DX12Device& device, const SamplerCreateInfo& createInfo);
        ~DX12Sampler() override;

        void Destroy() override;

        ID3D12DescriptorHeap* GetDX12DescriptorHeap();
        CD3DX12_CPU_DESCRIPTOR_HANDLE GetDX12CpuDescriptorHandle();
        CD3DX12_GPU_DESCRIPTOR_HANDLE GetDX12GpuDescriptorHandle();

    private:
        void CreateDX12Descriptor(DX12Device& device, const SamplerCreateInfo& createInfo);

        ID3D12DescriptorHeap* dx12DescriptorHeap;
        CD3DX12_CPU_DESCRIPTOR_HANDLE dx12CpuDescriptorHandle;
        CD3DX12_GPU_DESCRIPTOR_HANDLE dx12GpuDescriptorHandle;
    };
}
