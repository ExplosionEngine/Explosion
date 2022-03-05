//
// Created by johnk on 5/3/2022.
//

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
    DX12Sampler::DX12Sampler(const SamplerCreateInfo* createInfo) : Sampler(createInfo)
    {
        CreateDesc(createInfo);
    }

    DX12Sampler::~DX12Sampler() = default;

    void DX12Sampler::Destroy()
    {
        delete this;
    }

    D3D12_STATIC_SAMPLER_DESC* DX12Sampler::GetDesc() const
    {
        return desc.get();
    }

    void DX12Sampler::CreateDesc(const SamplerCreateInfo* createInfo)
    {
        desc = std::make_unique<D3D12_STATIC_SAMPLER_DESC>();
        desc->Filter = GetDX12Filter(createInfo);
        // TODO
    }
}
