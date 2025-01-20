//
// Created by johnk on 6/3/2022.
//

#include <RHI/DirectX12/Common.h>
#include <RHI/DirectX12/BindGroupLayout.h>

namespace RHI::DirectX12 {
    D3D12_SHADER_VISIBILITY GetShaderVisibility(const ShaderStageFlags shaderStageFlags)
    {
        uint8_t count = 0;
        Common::ForEachBits<ShaderStageBits>([&](ShaderStageBits shaderStage) -> void {
            if (shaderStageFlags & shaderStage) {
                count++;
            }
        });
        return count == 1 ? EnumCast<ShaderStageBits, D3D12_SHADER_VISIBILITY>(static_cast<ShaderStageBits>(shaderStageFlags.Value())) : D3D12_SHADER_VISIBILITY_ALL;
    }
}

namespace RHI::DirectX12 {
    RootParameterKeyInfo::RootParameterKeyInfo(const BindingType inBindingType, const uint8_t inLayoutIndex, const HlslBinding inBinding)
        : bindingType(inBindingType)
        , layoutIndex(inLayoutIndex)
        , binding(inBinding)
    {
    }

    DX12BindGroupLayout::DX12BindGroupLayout(const BindGroupLayoutCreateInfo& inCreateInfo)
        : BindGroupLayout(inCreateInfo)
        , layoutIndex(inCreateInfo.layoutIndex)
    {
        CreateNativeRootParameters(inCreateInfo);
    }

    DX12BindGroupLayout::~DX12BindGroupLayout() = default;

    uint8_t DX12BindGroupLayout::GetLayoutIndex() const
    {
        return layoutIndex;
    }

    const std::vector<RootParameterKeyInfo>& DX12BindGroupLayout::GetRootParameterKeyInfos() const
    {
        return rootParameterKeyInfos;
    }

    const std::vector<CD3DX12_ROOT_PARAMETER1>& DX12BindGroupLayout::GetNativeRootParameters() const
    {
        return nativeRootParameters;
    }

    void DX12BindGroupLayout::CreateNativeRootParameters(const BindGroupLayoutCreateInfo& inCreateInfo)
    {
        const auto entryCount = inCreateInfo.entries.size();
        nativeDescriptorRanges.reserve(entryCount);
        for (auto i = 0; i < entryCount; i++) {
            const auto& entry = inCreateInfo.entries[i];

            nativeRootParameters.emplace_back();
            nativeDescriptorRanges.emplace_back();

            const auto& hlslBinding = std::get<HlslBinding>(entry.binding.platformBinding);
            nativeDescriptorRanges.back().Init(EnumCast<HlslBindingRangeType, D3D12_DESCRIPTOR_RANGE_TYPE>(hlslBinding.rangeType), 1, hlslBinding.index, inCreateInfo.layoutIndex);
            nativeRootParameters.back().InitAsDescriptorTable(1, &nativeDescriptorRanges.back(), GetShaderVisibility(entry.shaderVisibility));

            rootParameterKeyInfos.emplace_back(entry.binding.type, inCreateInfo.layoutIndex, hlslBinding);
        }
    }
}
