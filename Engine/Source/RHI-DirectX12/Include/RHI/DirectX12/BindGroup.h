//
// Created by johnk on 20/3/2022.
//

#pragma once

#include <utility>
#include <vector>
#include <unordered_set>

#include <directx/d3dx12.h>

#include <RHI/BindGroup.h>

namespace RHI::DirectX12 {
    class DX12BindGroupLayout;

    class DX12BindGroup : public BindGroup {
    public:
        NON_COPYABLE(DX12BindGroup)
        explicit DX12BindGroup(const BindGroupCreateInfo* createInfo);
        ~DX12BindGroup() override;

        void Destroy() override;

        DX12BindGroupLayout& GetBindGroupLayout();
        const std::vector<ID3D12DescriptorHeap*>& GetDX12DescriptorHeaps();
        const std::vector<std::pair<uint8_t, std::pair<BindingType, CD3DX12_GPU_DESCRIPTOR_HANDLE>>>& GetBindings();

    private:
        void SaveBindGroupLayout(const BindGroupCreateInfo* createInfo);
        void CacheBindings(const BindGroupCreateInfo* createInfo);

        DX12BindGroupLayout* bindGroupLayout;
        std::vector<ID3D12DescriptorHeap*> dx12DescriptorHeaps;
        std::vector<std::pair<uint8_t, std::pair<BindingType, CD3DX12_GPU_DESCRIPTOR_HANDLE>>> bindings;
    };
}
