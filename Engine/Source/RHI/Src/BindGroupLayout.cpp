//
// Created by johnk on 19/2/2022.
//

#include <RHI/BindGroupLayout.h>

namespace RHI {
    HlslBinding::HlslBinding(HlslBindingRangeType inRangeType, uint8_t inIndex)
        : rangeType(inRangeType)
        , index(inIndex)
    {
    }

    GlslBinding::GlslBinding(uint8_t inIndex)
        : index(inIndex)
    {
    }

    ResourceBinding::ResourceBinding(BindingType inType, const std::variant<HlslBinding, GlslBinding>& inPlatformBinding)
        : type(inType)
        , platformBinding(inPlatformBinding)
    {
    }

    BindGroupLayoutEntry::BindGroupLayoutEntry(const ResourceBinding& inBinding, ShaderStageFlags inShaderVisibility)
        : binding(inBinding)
        , shaderVisibility(inShaderVisibility)
    {
    }

    BindGroupLayoutCreateInfo::BindGroupLayoutCreateInfo(uint8_t inLayoutIndex, std::string inDebugName)
        : layoutIndex(inLayoutIndex)
        , debugName(std::move(inDebugName))
    {
    }

    BindGroupLayoutCreateInfo& BindGroupLayoutCreateInfo::Entry(const BindGroupLayoutEntry& inEntry)
    {
        entries.emplace_back(inEntry);
        return *this;
    }

    BindGroupLayout::BindGroupLayout(const BindGroupLayoutCreateInfo& createInfo) {}

    BindGroupLayout::~BindGroupLayout() = default;
}
