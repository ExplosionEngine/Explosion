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
        if (minFilter == FilterMode::nearest && magFilter == FilterMode::nearest && mipFilter == FilterMode::nearest) { return D3D12_FILTER_MIN_MAG_MIP_POINT; }
        if (minFilter == FilterMode::nearest && magFilter == FilterMode::nearest && mipFilter == FilterMode::linear) { return D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR; }
        if (minFilter == FilterMode::nearest && magFilter == FilterMode::linear && mipFilter == FilterMode::nearest) { return D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT; }
        if (minFilter == FilterMode::nearest && magFilter == FilterMode::linear && mipFilter == FilterMode::linear) { return D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR; }
        if (minFilter == FilterMode::linear && magFilter == FilterMode::nearest && mipFilter == FilterMode::nearest) { return D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT; }
        if (minFilter == FilterMode::linear && magFilter == FilterMode::nearest && mipFilter == FilterMode::linear) { return D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR; }
        if (minFilter == FilterMode::linear && magFilter == FilterMode::linear && mipFilter == FilterMode::nearest) { return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT; }
        if (minFilter == FilterMode::linear && magFilter == FilterMode::linear && mipFilter == FilterMode::linear) { return D3D12_FILTER_MIN_MAG_MIP_LINEAR; }
        return D3D12_FILTER_MIN_MAG_MIP_POINT;
    }
}

namespace RHI::DirectX12 {
    DX12Sampler::DX12Sampler(DX12Device& inDevice, const SamplerCreateInfo& inCreateInfo)
        : Sampler(inCreateInfo)
        , descriptorAllocation()
    {
        CreateDX12Descriptor(inDevice, inCreateInfo);
    }

    DX12Sampler::~DX12Sampler() = default;

    CD3DX12_CPU_DESCRIPTOR_HANDLE DX12Sampler::GetNativeCpuDescriptorHandle()
    {
        return descriptorAllocation->GetCpuHandle();
    }

    void DX12Sampler::CreateDX12Descriptor(DX12Device& inDevice, const SamplerCreateInfo& inCreateInfo)
    {
        D3D12_SAMPLER_DESC desc {};
        desc.AddressU = EnumCast<AddressMode, D3D12_TEXTURE_ADDRESS_MODE>(inCreateInfo.addressModeU);
        desc.AddressV = EnumCast<AddressMode, D3D12_TEXTURE_ADDRESS_MODE>(inCreateInfo.addressModeV);
        desc.AddressW = EnumCast<AddressMode, D3D12_TEXTURE_ADDRESS_MODE>(inCreateInfo.addressModeW);
        desc.Filter = GetDX12Filter(inCreateInfo);
        desc.MinLOD = inCreateInfo.lodMinClamp;
        desc.MaxLOD = inCreateInfo.lodMaxClamp;
        desc.ComparisonFunc = EnumCast<CompareFunc, D3D12_COMPARISON_FUNC>(inCreateInfo.comparisonFunc);
        desc.MaxAnisotropy = inCreateInfo.maxAnisotropy;

        descriptorAllocation = inDevice.AllocateSamplerDescriptor();
        inDevice.GetNative()->CreateSampler(&desc, descriptorAllocation->GetCpuHandle());
    }
}
