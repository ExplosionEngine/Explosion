//
// Created by johnk on 6/3/2022.
//

#include <unordered_map>

#include <RHI/DirectX12/Common.h>
#include <RHI/DirectX12/BindGroupLayout.h>

namespace RHI::DirectX12 {
    D3D12_SHADER_VISIBILITY GetShaderVisibility(ShaderStageFlags shaderStageFlags)
    {
        uint8_t count = 0;
        ForEachBitsType<ShaderStageBits>([&](ShaderStageBits shaderStage) -> void {
            if (shaderStageFlags & shaderStage) {
                count++;
            }
        });
        return count == 1 ? DX12EnumCast<ShaderStageBits, D3D12_SHADER_VISIBILITY>(static_cast<ShaderStageBits>(shaderStageFlags.Value())) : D3D12_SHADER_VISIBILITY_ALL;
    }
}

namespace RHI::DirectX12 {
    DX12BindGroupLayout::DX12BindGroupLayout(const BindGroupLayoutCreateInfo& createInfo)
        : BindGroupLayout(createInfo), dx12RootParameters({}), layoutIndex(createInfo.layoutIndex)
    {
        CreateDX12RootParameters(createInfo);
    }

    DX12BindGroupLayout::~DX12BindGroupLayout() = default;

    void DX12BindGroupLayout::Destroy()
    {
        delete this;
    }

    uint8_t DX12BindGroupLayout::GetLayoutIndex() const
    {
        return layoutIndex;
    }

    const std::vector<RootParameterKeyInfo>& DX12BindGroupLayout::GetRootParameterKeyInfos() const
    {
        return rootParameterKeyInfos;
    }

    const std::vector<CD3DX12_ROOT_PARAMETER1>& DX12BindGroupLayout::GetDX12RootParameters() const
    {
        return dx12RootParameters;
    }

    void DX12BindGroupLayout::CreateDX12RootParameters(const BindGroupLayoutCreateInfo& createInfo)
    {
        dx12DescriptorRanges.reserve(createInfo.entryNum);
        for (auto i = 0; i < createInfo.entryNum; i++) {
            const auto& entry = createInfo.entries[i];

            dx12RootParameters.emplace_back();
            dx12DescriptorRanges.emplace_back();

            dx12DescriptorRanges.back().Init(DX12EnumCast<HlslBindingRangeType, D3D12_DESCRIPTOR_RANGE_TYPE>(entry.binding.platform.hlsl.rangeType), 1, entry.binding.platform.hlsl.index, createInfo.layoutIndex);
            dx12RootParameters.back().InitAsDescriptorTable(1, &dx12DescriptorRanges.back(), GetShaderVisibility(entry.shaderVisibility));

            rootParameterKeyInfos.emplace_back();
            {
                auto& keyInfo = rootParameterKeyInfos.back();
                keyInfo.bindingType = entry.binding.type;
                keyInfo.layoutIndex = createInfo.layoutIndex;
                keyInfo.binding = entry.binding.platform.hlsl;
            }
        }
    }
}
