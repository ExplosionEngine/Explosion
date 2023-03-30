//
// Created by johnk on 5/3/2022.
//

#include <RHI/DirectX12/Common.h>
#include <RHI/DirectX12/Device.h>
#include <RHI/DirectX12/Sampler.h>

namespace RHI::DirectX12 {
    static D3D12_FILTER GetDX12Filter(const SamplerCreateInfo& createInfo)
    {
        const auto& minFilter = createInfo.minFilter;
        const auto& magFilter = createInfo.magFilter;
        const auto& mipFilter = createInfo.mipFilter;
        if (minFilter == FilterMode::NEAREST && magFilter == FilterMode::NEAREST && mipFilter == FilterMode::NEAREST) { return D3D12_FILTER_MIN_MAG_MIP_POINT; }
        if (minFilter == FilterMode::NEAREST && magFilter == FilterMode::NEAREST && mipFilter == FilterMode::LINEAR) { return D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR; }
        if (minFilter == FilterMode::NEAREST && magFilter == FilterMode::LINEAR && mipFilter == FilterMode::NEAREST) { return D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT; }
        if (minFilter == FilterMode::NEAREST && magFilter == FilterMode::LINEAR && mipFilter == FilterMode::LINEAR) { return D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR; }
        if (minFilter == FilterMode::LINEAR && magFilter == FilterMode::NEAREST && mipFilter == FilterMode::NEAREST) { return D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT; }
        if (minFilter == FilterMode::LINEAR && magFilter == FilterMode::NEAREST && mipFilter == FilterMode::LINEAR) { return D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR; }
        if (minFilter == FilterMode::LINEAR && magFilter == FilterMode::LINEAR && mipFilter == FilterMode::NEAREST) { return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT; }
        if (minFilter == FilterMode::LINEAR && magFilter == FilterMode::LINEAR && mipFilter == FilterMode::LINEAR) { return D3D12_FILTER_MIN_MAG_MIP_LINEAR; }
        return D3D12_FILTER_MIN_MAG_MIP_POINT;
    }
}

namespace RHI::DirectX12 {
    DX12Sampler::DX12Sampler(DX12Device& device, const SamplerCreateInfo& createInfo)
        : Sampler(createInfo), dx12DescriptorHeap(nullptr), dx12CpuDescriptorHandle(), dx12GpuDescriptorHandle()
    {
        CreateDX12Descriptor(device, createInfo);
    }

    DX12Sampler::~DX12Sampler() = default;

    void DX12Sampler::Destroy()
    {
        delete this;
    }

    CD3DX12_CPU_DESCRIPTOR_HANDLE DX12Sampler::GetDX12CpuDescriptorHandle()
    {
        return dx12CpuDescriptorHandle;
    }

    CD3DX12_GPU_DESCRIPTOR_HANDLE DX12Sampler::GetDX12GpuDescriptorHandle()
    {
        return dx12GpuDescriptorHandle;
    }

    ID3D12DescriptorHeap* DX12Sampler::GetDX12DescriptorHeap()
    {
        return dx12DescriptorHeap;
    }

    void DX12Sampler::CreateDX12Descriptor(DX12Device& device, const SamplerCreateInfo& createInfo)
    {
        D3D12_SAMPLER_DESC desc {};
        desc.AddressU = DX12EnumCast<AddressMode, D3D12_TEXTURE_ADDRESS_MODE>(createInfo.addressModeU);
        desc.AddressV = DX12EnumCast<AddressMode, D3D12_TEXTURE_ADDRESS_MODE>(createInfo.addressModeV);
        desc.AddressW = DX12EnumCast<AddressMode, D3D12_TEXTURE_ADDRESS_MODE>(createInfo.addressModeW);
        desc.Filter = GetDX12Filter(createInfo);
        desc.MinLOD = createInfo.lodMinClamp;
        desc.MaxLOD = createInfo.lodMaxClamp;
        desc.ComparisonFunc = DX12EnumCast<ComparisonFunc, D3D12_COMPARISON_FUNC>(createInfo.comparisonFunc);
        desc.MaxAnisotropy = createInfo.maxAnisotropy;

        auto allocation = device.AllocateSamplerDescriptor();
        dx12CpuDescriptorHandle = allocation.cpuHandle;
        dx12GpuDescriptorHandle = allocation.gpuHandle;
        dx12DescriptorHeap = allocation.descriptorHeap;
        device.GetDX12Device()->CreateSampler(&desc, dx12CpuDescriptorHandle);
    }
}
