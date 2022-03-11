//
// Created by johnk on 11/3/2022.
//

#include <vector>

#include <directx/d3dx12.h>

#include <RHI/DirectX12/Common.h>
#include <RHI/DirectX12/Device.h>
#include <RHI/DirectX12/PipelineLayout.h>
#include <RHI/DirectX12/BindGroupLayout.h>

namespace RHI::DirectX12 {
    DX12PipelineLayout::DX12PipelineLayout(DX12Device& device, const PipelineLayoutCreateInfo* createInfo) : PipelineLayout(createInfo)
    {
        CreateRootSignature(device, createInfo);
    }

    DX12PipelineLayout::~DX12PipelineLayout() = default;

    void DX12PipelineLayout::Destroy()
    {
        delete this;
    }

    ComPtr<ID3D12RootSignature>& DX12PipelineLayout::GetDX12RootSignature()
    {
        return dx12RootSignature;
    }

    void DX12PipelineLayout::CreateRootSignature(DX12Device& device, const PipelineLayoutCreateInfo* createInfo)
    {
        D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

        std::vector<CD3DX12_ROOT_PARAMETER1> rootParameters;
        {
            for (auto i = 0; i < createInfo->bindGroupNum; i++) {
                auto* bindGroupLayout = dynamic_cast<const DX12BindGroupLayout*>(createInfo->bindGroupLayouts + i);
                const auto& pendingRootParameters = bindGroupLayout->GetDX12RootParameters();
                for (const auto& pendingRootParameter : pendingRootParameters) {
                    rootParameters.emplace_back(pendingRootParameter);
                }
            }
        }
        // TODO root constants

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init_1_1(rootParameters.size(), rootParameters.data(), 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        if (FAILED(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error))) {
            throw DX12Exception("failed to serialize versioned root signature");
        }
        if (FAILED(device.GetDX12Device()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&dx12RootSignature)))) {
            throw DX12Exception("failed to create root signature");
        }
    }
}
