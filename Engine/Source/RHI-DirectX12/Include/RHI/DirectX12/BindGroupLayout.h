//
// Created by johnk on 6/3/2022.
//

#pragma once

#include <memory>

#include <directx/d3dx12.h>

#include <RHI/BindGroupLayout.h>

namespace RHI::DirectX12 {
    class DX12Device;

    struct RootParameterKeyInfo {
        BindingType bindingType;
        uint8_t layoutIndex;
        HlslBinding binding;

        RootParameterKeyInfo(BindingType inBindingType, uint8_t inLayoutIndex, HlslBinding inBinding);
    };

    class DX12BindGroupLayout : public BindGroupLayout {
    public:
        NonCopyable(DX12BindGroupLayout)
        explicit DX12BindGroupLayout(const BindGroupLayoutCreateInfo& inCreateInfo);
        ~DX12BindGroupLayout() override;

        uint8_t GetLayoutIndex() const;
        [[nodiscard]] const std::vector<RootParameterKeyInfo>& GetRootParameterKeyInfos() const;
        [[nodiscard]] const std::vector<CD3DX12_ROOT_PARAMETER1>& GetNativeRootParameters() const;

    private:
        void CreateNativeRootParameters(const BindGroupLayoutCreateInfo& inCreateInfo);

        uint8_t layoutIndex;
        std::vector<RootParameterKeyInfo> rootParameterKeyInfos;
        std::vector<CD3DX12_ROOT_PARAMETER1> nativeRootParameters;
        std::vector<CD3DX12_DESCRIPTOR_RANGE1> nativeDescriptorRanges;
    };
}
