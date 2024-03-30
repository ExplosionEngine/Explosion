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

        bool operator==(const RootParameterKey& inOther) const;
    };

    struct RootParameterKeyHashProvider {
        size_t operator()(const RootParameterKey& inKey) const;
    };

    using RootParameterIndex = uint32_t;
    using BindingTypeAndRootParameterIndex = std::pair<BindingType, uint32_t>;
    using RootParameterIndexMap = std::unordered_map<RootParameterKey, BindingTypeAndRootParameterIndex, RootParameterKeyHashProvider>;

    class DX12PipelineLayout : public PipelineLayout {
    public:
        NonCopyable(DX12PipelineLayout)
        DX12PipelineLayout(DX12Device& inDevice, const PipelineLayoutCreateInfo& inCreateInfo);
        ~DX12PipelineLayout() override;

        void Destroy() override;

        std::optional<BindingTypeAndRootParameterIndex> QueryRootDescriptorParameterIndex(uint8_t inLayoutIndex, const HlslBinding& inBinding);
        ID3D12RootSignature* GetNative();

    private:
        void CreateNativeRootSignature(DX12Device& inDevice, const PipelineLayoutCreateInfo& inCreateInfo);

        ComPtr<ID3D12RootSignature> nativeRootSignature;
        RootParameterIndexMap rootParameterIndexMap;
    };
}
