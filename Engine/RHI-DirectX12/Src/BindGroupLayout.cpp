//
// Created by johnk on 6/3/2022.
//

#include <functional>
#include <unordered_map>

#include <RHI/DirectX12/Common.h>
#include <RHI/DirectX12/Device.h>
#include <RHI/DirectX12/BindGroupLayout.h>

namespace RHI::DirectX12 {
    DX12BindGroupLayout::DX12BindGroupLayout(DX12Device& device, const BindGroupLayoutCreateInfo* createInfo)
        : BindGroupLayout(createInfo), dx12DescriptorRanges({}), dx12RootParameters({})
    {
        CreateDX12RootSignatureDesc(device, createInfo);
    }

    DX12BindGroupLayout::~DX12BindGroupLayout() = default;

    void DX12BindGroupLayout::Destroy()
    {
        delete this;
    }

    const std::vector<CD3DX12_ROOT_PARAMETER1>& DX12BindGroupLayout::GetDX12RootParameters()
    {
        return dx12RootParameters;
    }

    void DX12BindGroupLayout::CreateDX12RootSignatureDesc(DX12Device& device, const BindGroupLayoutCreateInfo* createInfo)
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
            auto lastRange = dx12DescriptorRanges.end();
            for (const auto* entry : visibility.second) {
                dx12DescriptorRanges.emplace_back();
                dx12DescriptorRanges.back().Init(DX12EnumCast<BindingType, D3D12_DESCRIPTOR_RANGE_TYPE>(entry->type), 1, entry->binding, createInfo->layoutIndex, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
            }
            auto newLastRange = dx12DescriptorRanges.end();
            dx12RootParameters.emplace_back();
            dx12RootParameters.back().InitAsDescriptorTable(newLastRange - lastRange, &*lastRange, DX12EnumCast<ShaderStageBits, D3D12_SHADER_VISIBILITY>(visibility.first));
        }
    }
}
