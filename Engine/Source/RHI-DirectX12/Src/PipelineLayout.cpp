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
    bool RootParameterKey::operator==(const RootParameterKey& inOther) const
    {
        return layoutIndex == inOther.layoutIndex
            && binding.rangeType == inOther.binding.rangeType
            && binding.index == inOther.binding.index;
    }

    size_t RootParameterKeyHashProvider::operator()(const RootParameterKey& inKey) const
    {
        return Common::HashUtils::CityHash(&inKey, sizeof(RootParameterKey));
    }

    DX12PipelineLayout::DX12PipelineLayout(DX12Device& inDevice, const PipelineLayoutCreateInfo& inCreateInfo) : PipelineLayout(inCreateInfo)
    {
        CreateNativeRootSignature(inDevice, inCreateInfo);
    }

    DX12PipelineLayout::~DX12PipelineLayout() = default;

    std::optional<BindingTypeAndRootParameterIndex> DX12PipelineLayout::QueryRootDescriptorParameterIndex(uint8_t inLayoutIndex, const HlslBinding& inBinding)
    {
        auto iter = rootParameterIndexMap.find(RootParameterKey {inLayoutIndex, inBinding });
        if (iter == rootParameterIndexMap.end()) {
            return {};
        }
        return iter->second;
    }

    ID3D12RootSignature* DX12PipelineLayout::GetNative()
    {
        return nativeRootSignature.Get();
    }

    void DX12PipelineLayout::CreateNativeRootSignature(DX12Device& inDevice, const PipelineLayoutCreateInfo& inCreateInfo)
    {
        D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

        std::vector<CD3DX12_ROOT_PARAMETER1> rootParameters;
        for (auto i = 0; i < inCreateInfo.bindGroupLayouts.size(); i++) {
            const auto* bindGroupLayout = static_cast<const DX12BindGroupLayout*>(inCreateInfo.bindGroupLayouts[i]);
            const auto baseIndex = static_cast<uint32_t>(rootParameters.size());

            const auto& pendingRootParameters = bindGroupLayout->GetNativeRootParameters();
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
        success = SUCCEEDED(inDevice.GetNative()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&nativeRootSignature)));
        Assert(success);

#if BUILD_CONFIG_DEBUG
        if (!inCreateInfo.debugName.empty()) {
            nativeRootSignature->SetName(Common::StringUtils::ToWideString(inCreateInfo.debugName).c_str());
        }
#endif
    }
}
