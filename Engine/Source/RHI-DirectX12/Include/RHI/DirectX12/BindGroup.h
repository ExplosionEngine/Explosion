//
// Created by johnk on 20/3/2022.
//

#pragma once

#include <utility>
#include <vector>
#include <unordered_set>

#include <directx/d3dx12.h>

#include <RHI/BindGroup.h>
#include <Common/Hash.h>

namespace RHI::DirectX12 {
    class DX12BindGroupLayout;

    class DX12BindGroup : public BindGroup {
    public:
        NonCopyable(DX12BindGroup)
        explicit DX12BindGroup(const BindGroupCreateInfo& inCreateInfo);
        ~DX12BindGroup() override;

        DX12BindGroupLayout& GetBindGroupLayout();
        const std::vector<std::pair<HlslBinding, CD3DX12_CPU_DESCRIPTOR_HANDLE>>& GetNativeBindings();

    private:
        void SaveBindGroupLayout(const BindGroupCreateInfo& inCreateInfo);
        void CacheBindings(const BindGroupCreateInfo& inCreateInfo);

        DX12BindGroupLayout* bindGroupLayout;
        std::vector<std::pair<HlslBinding, CD3DX12_CPU_DESCRIPTOR_HANDLE>> nativeBindings;
    };
}
