//
// Created by johnk on 19/2/2022.
//

#pragma once

#include <Common/Utility.h>
#include <RHI/Common.h>

namespace RHI {
    struct HlslBinding {
        HlslBindingRangeType rangeType;
        uint8_t index;
    };

    struct GlslBinding {
        uint8_t index;
    };

    struct BindGroupLayoutEntry {
        BindingType type;
        union {
            HlslBinding hlsl;
            GlslBinding glsl;
        } binding;
        ShaderStageFlags shaderVisibility;
    };

    struct BindGroupLayoutCreateInfo {
        uint8_t layoutIndex;
        uint32_t entryNum;
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
