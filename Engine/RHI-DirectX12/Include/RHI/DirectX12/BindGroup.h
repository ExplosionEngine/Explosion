//
// Created by johnk on 20/3/2022.
//

#ifndef EXPLOSION_RHI_DX12_BIND_GROUP_H
#define EXPLOSION_RHI_DX12_BIND_GROUP_H

#include <utility>
#include <vector>
#include <unordered_set>

#include <directx/d3dx12.h>

#include <RHI/BindGroup.h>

namespace RHI::DirectX12 {
    class DX12BindGroup : public BindGroup {
    public:
        NON_COPYABLE(DX12BindGroup)
        explicit DX12BindGroup(const BindGroupCreateInfo* createInfo);
        ~DX12BindGroup() override;

        void Destroy() override;

        const std::unordered_set<ID3D12DescriptorHeap*>& GetDX12DescriptorHeaps();
        const std::vector<std::pair<uint8_t, CD3DX12_GPU_DESCRIPTOR_HANDLE>>& GetBindings();

    private:
        void CacheBindings(const BindGroupCreateInfo* createInfo);

        std::unordered_set<ID3D12DescriptorHeap*> dx12DescriptorHeaps;
        std::vector<std::pair<uint8_t, CD3DX12_GPU_DESCRIPTOR_HANDLE>> bindings;
    };
}

#endif//EXPLOSION_RHI_DX12_BIND_GROUP_H
