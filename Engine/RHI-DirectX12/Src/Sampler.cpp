//
// Created by johnk on 5/3/2022.
//

#include <RHI/DirectX12/Common.h>
#include <RHI/DirectX12/Sampler.h>

namespace RHI::DirectX12 {
    static D3D12_FILTER GetDX12Filter(const SamplerCreateInfo* createInfo)
    {
        const auto& minFilter = createInfo->minFilter;
        const auto& magFilter = createInfo->magFilter;
        const auto& mipFilter = createInfo->mipFilter;
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
    DX12Sampler::DX12Sampler(const SamplerCreateInfo* createInfo) : Sampler(createInfo), dx12SamplerDesc({})
    {
        CreateDX12SamplerDesc(createInfo);
    }

    DX12Sampler::~DX12Sampler() = default;

    void DX12Sampler::Destroy()
    {
        delete this;
    }

    D3D12_STATIC_SAMPLER_DESC* DX12Sampler::GetDX12SamplerDesc()
    {
        return &dx12SamplerDesc;
    }

    void DX12Sampler::CreateDX12SamplerDesc(const SamplerCreateInfo* createInfo)
    {
        dx12SamplerDesc.AddressU = DX12EnumCast<AddressMode, D3D12_TEXTURE_ADDRESS_MODE>(createInfo->addressModeU);
        dx12SamplerDesc.AddressV = DX12EnumCast<AddressMode, D3D12_TEXTURE_ADDRESS_MODE>(createInfo->addressModeV);
        dx12SamplerDesc.AddressW = DX12EnumCast<AddressMode, D3D12_TEXTURE_ADDRESS_MODE>(createInfo->addressModeW);
        dx12SamplerDesc.Filter = GetDX12Filter(createInfo);
        dx12SamplerDesc.MinLOD = createInfo->lodMinClamp;
        dx12SamplerDesc.MaxLOD = createInfo->lodMaxClamp;
        dx12SamplerDesc.ComparisonFunc = DX12EnumCast<ComparisonFunc, D3D12_COMPARISON_FUNC>(createInfo->comparisonFunc);
        dx12SamplerDesc.MaxAnisotropy = createInfo->maxAnisotropy;
    }
}
