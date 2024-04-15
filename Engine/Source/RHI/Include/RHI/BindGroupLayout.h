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

        HlslBinding(HlslBindingRangeType inRangeType, uint8_t inIndex);
    };

    struct GlslBinding {
        uint8_t index;

        explicit GlslBinding(uint8_t inIndex);
    };

    struct ResourceBinding {
        BindingType type;
        std::variant<HlslBinding, GlslBinding> platformBinding;

        ResourceBinding(BindingType inType, const std::variant<HlslBinding, GlslBinding>& inPlatformBinding);
    };

    struct BindGroupLayoutEntry {
        ResourceBinding binding;
        ShaderStageFlags shaderVisibility;

        BindGroupLayoutEntry(const ResourceBinding& inBinding, ShaderStageFlags inShaderVisibility);
    };

    struct BindGroupLayoutCreateInfo {
        uint8_t layoutIndex;
        std::vector<BindGroupLayoutEntry> entries;
        std::string debugName;

        explicit BindGroupLayoutCreateInfo(uint8_t inLayoutIndex, std::string inDebugName = "");
        BindGroupLayoutCreateInfo& AddEntry(const BindGroupLayoutEntry& inEntry);
    };

    class BindGroupLayout {
    public:
        NonCopyable(BindGroupLayout)
        virtual ~BindGroupLayout();

    protected:
        explicit BindGroupLayout(const BindGroupLayoutCreateInfo& createInfo);
    };
}
