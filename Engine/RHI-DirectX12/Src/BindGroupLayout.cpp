//
// Created by johnk on 6/3/2022.
//

#include <RHI/DirectX12/Device.h>
#include <RHI/DirectX12/BindGroupLayout.h>

namespace RHI::DirectX12 {
    DX12BindGroupLayout::DX12BindGroupLayout(DX12Device& device, const BindGroupLayoutCreateInfo* createInfo)
        : BindGroupLayout(createInfo), dx12RootSignatureFeatureData({}), dx12RootSignatureDesc(), dx12DescriptorRanges({}), dx12RootParameters({})
    {
        CreateDX12RootSignatureFeatureData(device, createInfo);
        CreateDX12DescriptorRanges(createInfo);
        CreateDX12RootParameters(createInfo);
        CreateDX12RootSignatureDesc(createInfo);
    }

    DX12BindGroupLayout::~DX12BindGroupLayout() = default;

    void DX12BindGroupLayout::Destroy()
    {
        delete this;
    }

    D3D12_VERSIONED_ROOT_SIGNATURE_DESC* DX12BindGroupLayout::GetDX12RootSignatureDesc()
    {
        return &dx12RootSignatureDesc;
    }

    void DX12BindGroupLayout::CreateDX12RootSignatureFeatureData(DX12Device& device, const BindGroupLayoutCreateInfo* createInfo)
    {
        dx12RootSignatureFeatureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
        if (FAILED(device.GetDX12Device()->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &dx12RootSignatureFeatureData, sizeof(D3D12_FEATURE_DATA_ROOT_SIGNATURE)))) {
            dx12RootSignatureFeatureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
        }
    }

    void DX12BindGroupLayout::CreateDX12DescriptorRanges(const BindGroupLayoutCreateInfo* createInfo)
    {
        // TODO
    }

    void DX12BindGroupLayout::CreateDX12RootParameters(const BindGroupLayoutCreateInfo* createInfo)
    {
        // TODO
    }

    void DX12BindGroupLayout::CreateDX12RootSignatureDesc(const BindGroupLayoutCreateInfo* createInfo)
    {
        // TODO
    }
}
