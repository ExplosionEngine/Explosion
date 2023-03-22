//
// Created by johnk on 11/3/2022.
//

#pragma once

#include <unordered_map>
#include <optional>

#include <wrl/client.h>
#include <d3d12.h>

#include <RHI/PipelineLayout.h>

using Microsoft::WRL::ComPtr;

namespace RHI::DirectX12 {
    class DX12Device;

    using LayoutIndexAndBinding = uint16_t;
    using RootParameterIndex = uint32_t;
    using BindingTypeAndRootParameterIndex = std::pair<BindingType, uint32_t>;
    using RootParameterIndexMap = std::unordered_map<LayoutIndexAndBinding, BindingTypeAndRootParameterIndex>;

    class DX12PipelineLayout : public PipelineLayout {
    public:
        NON_COPYABLE(DX12PipelineLayout)
        DX12PipelineLayout(DX12Device& device, const PipelineLayoutCreateInfo* createInfo);
        ~DX12PipelineLayout() override;

        void Destroy() override;

        std::optional<BindingTypeAndRootParameterIndex> QueryRootDescriptorParameterIndex(ShaderStageBits shaderStage, uint8_t layoutIndex, uint8_t binding, BindingType type);
        ComPtr<ID3D12RootSignature>& GetDX12RootSignature();

    private:
        void CreateDX12RootSignature(DX12Device& device, const PipelineLayoutCreateInfo* createInfo);

        ComPtr<ID3D12RootSignature> dx12RootSignature;
        std::unordered_map<ShaderStageBits, RootParameterIndexMap> rootDescriptorParameterIndexMaps;
    };
}
