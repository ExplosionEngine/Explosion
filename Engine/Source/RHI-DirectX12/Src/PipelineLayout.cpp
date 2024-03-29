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
    DX12PipelineLayout::DX12PipelineLayout(DX12Device& device, const PipelineLayoutCreateInfo& createInfo) : PipelineLayout(createInfo)
    {
        CreateDX12RootSignature(device, createInfo);
    }

    DX12PipelineLayout::~DX12PipelineLayout() = default;

    void DX12PipelineLayout::Destroy()
    {
        delete this;
    }

    std::optional<BindingTypeAndRootParameterIndex> DX12PipelineLayout::QueryRootDescriptorParameterIndex(uint8_t layoutIndex, const HlslBinding& binding)
    {
        auto iter = rootParameterIndexMap.find(RootParameterKey { layoutIndex, binding });
        if (iter == rootParameterIndexMap.end()) {
            return {};
        }
        return iter->second;
    }

    ComPtr<ID3D12RootSignature>& DX12PipelineLayout::GetDX12RootSignature()
    {
        return dx12RootSignature;
    }

    void DX12PipelineLayout::CreateDX12RootSignature(DX12Device& device, const PipelineLayoutCreateInfo& createInfo)
    {
        D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

        std::vector<CD3DX12_ROOT_PARAMETER1> rootParameters;
        for (auto i = 0; i < createInfo.bindGroupLayouts.size(); i++) {
            const auto* bindGroupLayout = static_cast<const DX12BindGroupLayout*>(createInfo.bindGroupLayouts[i]);
            const auto baseIndex = static_cast<uint32_t>(rootParameters.size());

            const auto& pendingRootParameters = bindGroupLayout->GetDX12RootParameters();
            const auto& keyInfos = bindGroupLayout->GetRootParameterKeyInfos();
            for (auto j = 0; j < pendingRootParameters.size(); j++) {
                const auto index = static_cast<uint32_t>(baseIndex + j);
                rootParameters.emplace_back(pendingRootParameters[j]);

                const auto& keyInfo = keyInfos[j];
                auto rootParameterKey = RootParameterKey { keyInfo.layoutIndex, keyInfo.binding };
                rootParameterIndexMap[rootParameterKey] = { keyInfo.bindingType, index };
            }
        }
        // TODO root constants

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init_1_1(rootParameters.size(), rootParameters.data(), 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        bool success = SUCCEEDED(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
        Assert(success);
        success = SUCCEEDED(device.GetDX12Device()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&dx12RootSignature)));
        Assert(success);

#if BUILD_CONFIG_DEBUG
        if (!createInfo.debugName.empty()) {
            dx12RootSignature->SetName(Common::StringUtils::ToWideString(createInfo.debugName).c_str());
        }
#endif
    }
}
