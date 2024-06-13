//
// Created by johnk on 19/2/2022.
//

#include <RHI/BindGroupLayout.h>

namespace RHI {
    HlslBinding::HlslBinding(const HlslBindingRangeType inRangeType, const uint8_t inIndex)
        : rangeType(inRangeType)
        , index(inIndex)
    {
    }

    GlslBinding::GlslBinding(const uint8_t inIndex)
        : index(inIndex)
    {
    }

    ResourceBinding::ResourceBinding(const BindingType inType, const std::variant<HlslBinding, GlslBinding>& inPlatformBinding)
        : type(inType)
        , platformBinding(inPlatformBinding)
    {
    }

    BindGroupLayoutEntry::BindGroupLayoutEntry(const ResourceBinding& inBinding, const ShaderStageFlags inShaderVisibility)
        : binding(inBinding)
        , shaderVisibility(inShaderVisibility)
    {
    }

    BindGroupLayoutCreateInfo::BindGroupLayoutCreateInfo(const uint8_t inLayoutIndex, std::string inDebugName)
        : layoutIndex(inLayoutIndex)
        , debugName(std::move(inDebugName))
    {
    }

    BindGroupLayoutCreateInfo& BindGroupLayoutCreateInfo::AddEntry(const BindGroupLayoutEntry& inEntry)
    {
        entries.emplace_back(inEntry);
        return *this;
    }

    BindGroupLayout::BindGroupLayout(const BindGroupLayoutCreateInfo&) {}

    BindGroupLayout::~BindGroupLayout() = default;
}
