//
// Created by johnk on 20/3/2022.
//

#include <RHI/DirectX12/BindGroup.h>
#include <RHI/DirectX12/BufferView.h>
#include <RHI/DirectX12/TextureView.h>
#include <RHI/DirectX12/Sampler.h>
#include <RHI/DirectX12/BindGroupLayout.h>
#include <RHI/DirectX12/Common.h>

namespace RHI::DirectX12 {
    static inline void GetDescriptorHandleAndHeap(CD3DX12_GPU_DESCRIPTOR_HANDLE& handle, ID3D12DescriptorHeap** heap, const BindGroupEntry& entry)
    {
        if (entry.binding.type == BindingType::uniformBuffer || entry.binding.type == BindingType::storageBuffer) {
            auto* bufferView = dynamic_cast<DX12BufferView*>(entry.bufferView);
            handle = bufferView->GetDX12GpuDescriptorHandle();
            *heap = bufferView->GetDX12DescriptorHeap();
        } else if (entry.binding.type == BindingType::texture || entry.binding.type == BindingType::storageTexture) {
            auto* textureView = dynamic_cast<DX12TextureView*>(entry.textureView);
            handle = textureView->GetDX12GpuDescriptorHandle();
            *heap = textureView->GetDX12DescriptorHeap();
        } else if (entry.binding.type == BindingType::sampler) {
            auto* sampler = dynamic_cast<DX12Sampler*>(entry.sampler);
            handle = sampler->GetDX12GpuDescriptorHandle();
            *heap = sampler->GetDX12DescriptorHeap();
        }
    }
}

namespace RHI::DirectX12 {
    DX12BindGroup::DX12BindGroup(const BindGroupCreateInfo& createInfo) : BindGroup(createInfo), bindGroupLayout(nullptr)
    {
        SaveBindGroupLayout(createInfo);
        CacheBindings(createInfo);
    }

    DX12BindGroup::~DX12BindGroup() = default;

    void DX12BindGroup::Destroy()
    {
        delete this;
    }

    DX12BindGroupLayout& DX12BindGroup::GetBindGroupLayout()
    {
        return *bindGroupLayout;
    }

    const std::vector<ID3D12DescriptorHeap*>& DX12BindGroup::GetDX12DescriptorHeaps()
    {
        return dx12DescriptorHeaps;
    }

    const std::vector<std::pair<HlslBinding, CD3DX12_GPU_DESCRIPTOR_HANDLE>>& DX12BindGroup::GetBindings()
    {
        return bindings;
    }

    void DX12BindGroup::SaveBindGroupLayout(const BindGroupCreateInfo& createInfo)
    {
        auto* tBindGroupLayout = dynamic_cast<DX12BindGroupLayout*>(createInfo.layout);
        Assert(tBindGroupLayout);
        bindGroupLayout = tBindGroupLayout;
    }

    void DX12BindGroup::CacheBindings(const BindGroupCreateInfo& createInfo)
    {
        for (auto i = 0; i < createInfo.entryNum; i++) {
            const auto& entry = createInfo.entries[i];

            CD3DX12_GPU_DESCRIPTOR_HANDLE handle;
            ID3D12DescriptorHeap* heap;
            GetDescriptorHandleAndHeap(handle, &heap, entry);

            dx12DescriptorHeaps.emplace_back(heap);
            bindings.emplace_back( entry.binding.platform.hlsl, handle );
        }
    }
}
