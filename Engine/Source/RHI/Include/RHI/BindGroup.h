//
// Created by johnk on 19/2/2022.
//

#pragma once

#include <cstddef>
#include <variant>
#include <utility>

#include <Common/Utility.h>
#include <RHI/Common.h>
#include <RHI/BindGroupLayout.h>

namespace RHI {
    class BufferView;
    class Sampler;
    class TextureView;

    struct BindGroupEntry {
        ResourceBinding binding;
        std::variant<Sampler*, BufferView*, TextureView*> entity;

        BindGroupEntry(const ResourceBinding& inBinding, const std::variant<Sampler*, BufferView*, TextureView*>& inEntity);
    };

    struct BindGroupCreateInfo {
        BindGroupLayout* layout;
        std::vector<BindGroupEntry> entries;
        std::string debugName;

        explicit BindGroupCreateInfo(BindGroupLayout* inLayout, std::string inDebugName = "");
        BindGroupCreateInfo& AddEntry(const BindGroupEntry& inEntry);
    };

    class BindGroup {
    public:
        NonCopyable(BindGroup)
        virtual ~BindGroup();

    protected:
        explicit BindGroup(const BindGroupCreateInfo& createInfo);
    };
}
