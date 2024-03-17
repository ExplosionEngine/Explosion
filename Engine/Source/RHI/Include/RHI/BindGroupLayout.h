//
// Created by johnk on 19/2/2022.
//

#pragma once

#include <variant>
#include <string>

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

    struct ResourceBinding {
        BindingType type;
        std::variant<HlslBinding, GlslBinding> platformBinding;
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
        NonCopyable(BindGroupLayout)
        virtual ~BindGroupLayout();

        virtual void Destroy() = 0;

    protected:
        explicit BindGroupLayout(const BindGroupLayoutCreateInfo& createInfo);
    };
}
