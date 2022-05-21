//
// Created by johnk on 19/2/2022.
//

#pragma once

#include <cstddef>

#include <Common/Utility.h>
#include <RHI/Common.h>

namespace RHI {
    class BindGroupLayout;
    class BufferView;
    class Sampler;
    class TextureView;

    struct BindGroupEntry {
        uint8_t binding;
        BindingType type;
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
    };

    class BindGroup {
    public:
        NON_COPYABLE(BindGroup)
        virtual ~BindGroup();

        virtual void Destroy() = 0;

    protected:
        explicit BindGroup(const BindGroupCreateInfo* createInfo);
    };
}
