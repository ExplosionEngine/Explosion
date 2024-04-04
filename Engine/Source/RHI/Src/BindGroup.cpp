//
// Created by johnk on 19/2/2022.
//

#include <RHI/BindGroup.h>

#include <utility>

namespace RHI {
    BindGroupEntry::BindGroupEntry(const ResourceBinding& inBinding, const std::variant<Sampler*, BufferView*, TextureView*>& inEntity)
        : binding(inBinding)
        , entity(inEntity)
    {
    }

    BindGroupCreateInfo::BindGroupCreateInfo(BindGroupLayout* inLayout, std::string inDebugName)
        : layout(inLayout)
        , debugName(std::move(inDebugName))
    {
    }

    BindGroupCreateInfo& BindGroupCreateInfo::AddEntry(const BindGroupEntry& inEntry)
    {
        entries.emplace_back(inEntry);
        return *this;
    }

    BindGroup::BindGroup(const BindGroupCreateInfo& createInfo) {}

    BindGroup::~BindGroup() = default;
}
