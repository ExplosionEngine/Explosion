//
// Created by johnk on 11/3/2022.
//

#pragma once

#include <unordered_map>
#include <optional>

#include <wrl/client.h>
#include <d3d12.h>

#include <RHI/PipelineLayout.h>
#include <Common/Hash.h>

using Microsoft::WRL::ComPtr;

namespace RHI::DirectX12 {
    class DX12Device;

    struct RootParameterKey {
        uint8_t layoutIndex;
        HlslBinding binding;

        bool operator==(const RootParameterKey& other) const
        {
            return layoutIndex == other.layoutIndex
                && binding.rangeType == other.binding.rangeType
                && binding.index == other.binding.index;
        }
    };

    struct RootParameterKeyHasher {
        size_t operator()(const RootParameterKey& key) const
        {
            return Common::HashUtils::CityHash(&key, sizeof(RootParameterKey));
        }
    };

    using RootParameterIndex = uint32_t;
    using BindingTypeAndRootParameterIndex = std::pair<BindingType, uint32_t>;
    using RootParameterIndexMap = std::unordered_map<RootParameterKey, BindingTypeAndRootParameterIndex, RootParameterKeyHasher>;

    class DX12PipelineLayout : public PipelineLayout {
    public:
        NON_COPYABLE(DX12PipelineLayout)
        DX12PipelineLayout(DX12Device& device, const PipelineLayoutCreateInfo& createInfo);
        ~DX12PipelineLayout() override;

        void Destroy() override;

        std::optional<BindingTypeAndRootParameterIndex> QueryRootDescriptorParameterIndex(ShaderStageBits shaderStage, uint8_t layoutIndex, const HlslBinding& binding);
        ComPtr<ID3D12RootSignature>& GetDX12RootSignature();

    private:
        void CreateDX12RootSignature(DX12Device& device, const PipelineLayoutCreateInfo& createInfo);

        ComPtr<ID3D12RootSignature> dx12RootSignature;
        std::unordered_map<ShaderStageBits, RootParameterIndexMap> rootDescriptorParameterIndexMaps;
    };
}
