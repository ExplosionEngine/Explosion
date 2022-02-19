//
// Created by johnk on 19/2/2022.
//

#ifndef EXPLOSION_RHI_BIND_GROUP_LAYOUT_H
#define EXPLOSION_RHI_BIND_GROUP_LAYOUT_H

#include <Common/Utility.h>
#include <RHI/Enum.h>

namespace RHI {
    struct BufferBindingLayout {
        BufferBindingType type;
        bool hasDynamicOffset;
        size_t minBindingSize;
    };

    struct SamplerBindingLayout {
        SamplerBindingType type;
    };

    struct TextureBindingLayout {
        TextureSampleType type;
        TextureViewDimension viewDimension;
        bool multiSampled;
    };

    struct StorageTextureBindingLayout {
        StorageTextureAccess access;
        PixelFormat format;
        TextureViewDimension viewDimension;
    };

    struct BindGroupLayoutEntry {
        size_t binding;
        ShaderStageFlags shaderVisibility;
        BufferBindingLayout buffer;
        SamplerBindingLayout sampler;
        TextureBindingLayout texture;
        StorageTextureBindingLayout storageTexture;
    };

    struct BindGroupLayoutCreateInfo {
        size_t entryNum;
        const BindGroupLayoutEntry* entries;
    };

    class BindGroupLayout {
    public:
        NON_COPYABLE(BindGroupLayout)
        virtual ~BindGroupLayout();

        virtual void Destroy() = 0;

    protected:
        explicit BindGroupLayout(const BindGroupLayoutCreateInfo* createInfo);
    };
}

#endif//EXPLOSION_RHI_BIND_GROUP_LAYOUT_H
