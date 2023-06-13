//
// Created by johnk on 19/2/2022.
//

#pragma once

#include <Common/Utility.h>
#include <RHI/Common.h>
#include <string>

namespace RHI {
    struct HlslBinding {
        HlslBindingRangeType rangeType;
        uint8_t index;
    };

    struct GlslBinding {
        uint8_t index;
    };

    struct ResourceBinding {
        BindingType type;
        union {
            HlslBinding hlsl;
            GlslBinding glsl;
        } platform;
    };

    struct BindGroupLayoutEntry {
        ResourceBinding binding;
        ShaderStageFlags shaderVisibility;
    };

    struct BindGroupLayoutCreateInfo {
        uint8_t layoutIndex;
        uint32_t entryNum;
        const BindGroupLayoutEntry* entries;
        std::string debugName;
    };

    class BindGroupLayout {
    public:
        NON_COPYABLE(BindGroupLayout)
        virtual ~BindGroupLayout();

        virtual void Destroy() = 0;

    protected:
        explicit BindGroupLayout(const BindGroupLayoutCreateInfo& createInfo);
    };
}
