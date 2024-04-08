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
    static inline CD3DX12_CPU_DESCRIPTOR_HANDLE GetDescriptorCpuHandle(const BindGroupEntry& entry)
    {
        if (entry.binding.type == BindingType::uniformBuffer || entry.binding.type == BindingType::storageBuffer) {
            auto* bufferView = static_cast<DX12BufferView*>(std::get<BufferView*>(entry.entity));
            return bufferView->GetNativeCpuDescriptorHandle();
        }
        if (entry.binding.type == BindingType::texture || entry.binding.type == BindingType::storageTexture) {
            auto* textureView = static_cast<DX12TextureView*>(std::get<TextureView*>(entry.entity));
            return textureView->GetNativeCpuDescriptorHandle();
        }
        if (entry.binding.type == BindingType::sampler) {
            auto* sampler = static_cast<DX12Sampler*>(std::get<Sampler*>(entry.entity));
            return sampler->GetNativeCpuDescriptorHandle();
        }
        QuickFail();
        return CD3DX12_CPU_DESCRIPTOR_HANDLE();
    }
}

namespace RHI::DirectX12 {
    DX12BindGroup::DX12BindGroup(const BindGroupCreateInfo& inCreateInfo) : BindGroup(inCreateInfo), bindGroupLayout(nullptr)
    {
        SaveBindGroupLayout(inCreateInfo);
        CacheBindings(inCreateInfo);
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

    const std::vector<std::pair<HlslBinding, CD3DX12_CPU_DESCRIPTOR_HANDLE>>& DX12BindGroup::GetNativeBindings()
    {
        return nativeBindings;
    }

    void DX12BindGroup::SaveBindGroupLayout(const BindGroupCreateInfo& inCreateInfo)
    {
        auto* tBindGroupLayout = static_cast<DX12BindGroupLayout*>(inCreateInfo.layout);
        Assert(tBindGroupLayout != nullptr);
        bindGroupLayout = tBindGroupLayout;
    }

    void DX12BindGroup::CacheBindings(const BindGroupCreateInfo& inCreateInfo)
    {
        for (auto i = 0; i < inCreateInfo.entries.size(); i++) {
            const auto& entry = inCreateInfo.entries[i];

            CD3DX12_CPU_DESCRIPTOR_HANDLE handle = GetDescriptorCpuHandle(entry);
            nativeBindings.emplace_back(std::get<HlslBinding>(entry.binding.platformBinding), handle);
        }
    }
}
