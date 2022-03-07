//
// Created by johnk on 6/3/2022.
//

#ifndef EXPLOSION_RHI_DX12_BIND_GROUP_LAYOUT_H
#define EXPLOSION_RHI_DX12_BIND_GROUP_LAYOUT_H

#include <memory>

#include <directx/d3dx12.h>

#include <RHI/BindGroupLayout.h>

namespace RHI::DirectX12 {
    class DX12Device;

    class DX12BindGroupLayout : public BindGroupLayout {
    public:
        NON_COPYABLE(DX12BindGroupLayout)
        DX12BindGroupLayout(DX12Device& device, const BindGroupLayoutCreateInfo* createInfo);
        ~DX12BindGroupLayout() override;

        void Destroy() override;

        D3D12_VERSIONED_ROOT_SIGNATURE_DESC* GetDX12RootSignatureDesc();

    private:
        void CreateDX12RootSignatureFeatureData(DX12Device& device, const BindGroupLayoutCreateInfo* createInfo);
        void CreateDX12DescriptorRanges(const BindGroupLayoutCreateInfo* createInfo);
        void CreateDX12RootParameters(const BindGroupLayoutCreateInfo* createInfo);
        void CreateDX12RootSignatureDesc(const BindGroupLayoutCreateInfo* createInfo);

        D3D12_FEATURE_DATA_ROOT_SIGNATURE dx12RootSignatureFeatureData;
        std::vector<CD3DX12_DESCRIPTOR_RANGE1> dx12DescriptorRanges;
        std::vector<CD3DX12_ROOT_PARAMETER1> dx12RootParameters;
        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC dx12RootSignatureDesc;
    };
}

#endif//EXPLOSION_RHI_DX12_BIND_GROUP_LAYOUT_H
