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
    static inline CD3DX12_CPU_DESCRIPTOR_HANDLE GetDescriptorCpuHandleAndHeap(const BindGroupEntry& entry)
    {
        if (entry.binding.type == BindingType::uniformBuffer || entry.binding.type == BindingType::storageBuffer) {
            auto* bufferView = static_cast<DX12BufferView*>(std::get<BufferView*>(entry.entity));
            return bufferView->GetDX12CpuDescriptorHandle();
        }
        if (entry.binding.type == BindingType::texture || entry.binding.type == BindingType::storageTexture) {
            auto* textureView = static_cast<DX12TextureView*>(std::get<TextureView*>(entry.entity));
            return textureView->GetDX12CpuDescriptorHandle();
        }
        if (entry.binding.type == BindingType::sampler) {
            auto* sampler = static_cast<DX12Sampler*>(std::get<Sampler*>(entry.entity));
            return sampler->GetDX12CpuDescriptorHandle();
        }
        QuickFail();
        return CD3DX12_CPU_DESCRIPTOR_HANDLE();
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

    const std::vector<std::pair<HlslBinding, CD3DX12_CPU_DESCRIPTOR_HANDLE>>& DX12BindGroup::GetBindings()
    {
        return bindings;
    }

    void DX12BindGroup::SaveBindGroupLayout(const BindGroupCreateInfo& createInfo)
    {
        auto* tBindGroupLayout = static_cast<DX12BindGroupLayout*>(createInfo.layout);
        Assert(tBindGroupLayout);
        bindGroupLayout = tBindGroupLayout;
    }

    void DX12BindGroup::CacheBindings(const BindGroupCreateInfo& createInfo)
    {
        for (auto i = 0; i < createInfo.entries.size(); i++) {
            const auto& entry = createInfo.entries[i];

            CD3DX12_CPU_DESCRIPTOR_HANDLE handle = GetDescriptorCpuHandleAndHeap(entry);
            bindings.emplace_back(std::get<HlslBinding>(entry.binding.platformBinding), handle);
        }
    }
}
