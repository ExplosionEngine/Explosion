//
// Created by johnk on 19/2/2022.
//

#ifndef EXPLOSION_RHI_BIND_GROUP_H
#define EXPLOSION_RHI_BIND_GROUP_H

#include <Common/Utility.h>

namespace RHI {
    class BindGroupLayout;
    class Buffer;
    class Sampler;
    class TextureView;

    struct BufferBinding {
        Buffer* buffer;
        size_t offset;
        size_t size;
    };

    struct BindGroupEntry {
        size_t binding;
        Sampler* sampler;
        TextureView* textureView;
        BufferBinding buffer;
    };

    struct BindGroupCreateInfo {
        BindGroupLayout* layout;
        size_t entryNum;
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

#endif//EXPLOSION_RHI_BIND_GROUP_H
