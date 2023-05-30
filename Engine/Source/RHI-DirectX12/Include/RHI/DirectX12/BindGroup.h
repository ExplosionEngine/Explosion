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
        NON_COPYABLE(DX12BindGroup)
        explicit DX12BindGroup(const BindGroupCreateInfo& createInfo);
        ~DX12BindGroup() override;

        void Destroy() override;

        DX12BindGroupLayout& GetBindGroupLayout();
        const std::vector<std::pair<HlslBinding, CD3DX12_CPU_DESCRIPTOR_HANDLE>>& GetBindings();

    private:
        void SaveBindGroupLayout(const BindGroupCreateInfo& createInfo);
        void CacheBindings(const BindGroupCreateInfo& createInfo);

        DX12BindGroupLayout* bindGroupLayout;
        std::vector<std::pair<HlslBinding, CD3DX12_CPU_DESCRIPTOR_HANDLE>> bindings;
    };
}
