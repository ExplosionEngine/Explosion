//
// Created by johnk on 6/3/2022.
//

#include <unordered_map>

#include <RHI/DirectX12/Common.h>
#include <RHI/DirectX12/BindGroupLayout.h>

namespace RHI::DirectX12 {
    DX12BindGroupLayout::DX12BindGroupLayout(const BindGroupLayoutCreateInfo* createInfo)
        : BindGroupLayout(createInfo), dx12DescriptorRanges({}), dx12RootParameters({})
    {
        CreateDX12RootParameters(createInfo);
    }

    DX12BindGroupLayout::~DX12BindGroupLayout() = default;

    void DX12BindGroupLayout::Destroy()
    {
        delete this;
    }

    const std::vector<CD3DX12_ROOT_PARAMETER1>& DX12BindGroupLayout::GetDX12RootParameters() const
    {
        return dx12RootParameters;
    }

    void DX12BindGroupLayout::CreateDX12RootParameters(const BindGroupLayoutCreateInfo* createInfo)
    {
        std::unordered_map<ShaderStageBits, std::vector<const BindGroupLayoutEntry*>> visibilitiesMap;
        {
            using UBitsType = std::underlying_type_t<ShaderStageBits>;
            for (UBitsType i = 0; i < static_cast<UBitsType>(ShaderStageBits::MAX); i = i << 1) {
                visibilitiesMap[static_cast<ShaderStageBits>(i)] = {};
            }
            for (auto i = 0; i < createInfo->entryNum; i++) {
                for (auto& visibility : visibilitiesMap) {
                    if (!(createInfo->entries[i].shaderVisibility & visibility.first)) {
                        continue;
                    }
                    visibility.second.emplace_back(createInfo->entries + i);
                }
            }
        }

        for (const auto& visibility : visibilitiesMap) {
            for (const auto* entry : visibility.second) {
                CD3DX12_DESCRIPTOR_RANGE1 dx12DescriptorRange;
                dx12DescriptorRange.Init(DX12EnumCast<BindingType, D3D12_DESCRIPTOR_RANGE_TYPE>(entry->type), 1, entry->binding, createInfo->layoutIndex, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
                dx12RootParameters.emplace_back();
                dx12RootParameters.back().InitAsDescriptorTable(1, &dx12DescriptorRange, DX12EnumCast<ShaderStageBits, D3D12_SHADER_VISIBILITY>(visibility.first));
            }
        }
    }
}
