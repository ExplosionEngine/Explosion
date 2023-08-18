//
// Created by johnk on 19/2/2022.
//

#pragma once

#include <cstddef>

#include <Common/Utility.h>
#include <RHI/Common.h>
#include <RHI/BindGroupLayout.h>

namespace RHI {
    class BufferView;
    class Sampler;
    class TextureView;

    struct BindGroupEntry {
        ResourceBinding binding;
        union {
            Sampler* sampler;
            TextureView* textureView;
            BufferView* bufferView;
        };
    };

    struct BindGroupCreateInfo {
        BindGroupLayout* layout;
        uint32_t entryNum;
        const BindGroupEntry* entries;
        std::string debugName;
    };

    class BindGroup {
    public:
        NonCopyable(BindGroup)
        virtual ~BindGroup();

        virtual void Destroy() = 0;

    protected:
        explicit BindGroup(const BindGroupCreateInfo& createInfo);
    };
}
