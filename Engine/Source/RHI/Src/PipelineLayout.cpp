//
// Created by johnk on 19/2/2022.
//

#include <RHI/PipelineLayout.h>

namespace RHI {
    PipelineConstantLayout::PipelineConstantLayout()
        : stageFlags(ShaderStageFlags::null)
        , offset(0)
        , size(0)
    {
    }

    PipelineConstantLayout::PipelineConstantLayout(const ShaderStageFlags inStageFlags, const uint32_t inOffset, const uint32_t inSize)
        : stageFlags(inStageFlags)
        , offset(inOffset)
        , size(inSize)
    {
    }

    PipelineConstantLayout& PipelineConstantLayout::SetStageFlags(const ShaderStageFlags inStageFlags)
    {
        stageFlags = inStageFlags;
        return *this;
    }

    PipelineConstantLayout& PipelineConstantLayout::SetOffset(const uint32_t inOffset)
    {
        offset = inOffset;
        return *this;
    }

    PipelineConstantLayout& PipelineConstantLayout::SetSize(const uint32_t inSize)
    {
        size = inSize;
        return *this;
    }

    PipelineLayoutCreateInfo::PipelineLayoutCreateInfo()
    {
    }

    PipelineLayoutCreateInfo& PipelineLayoutCreateInfo::AddBindGroupLayout(const BindGroupLayout* inLayout)
    {
        bindGroupLayouts.emplace_back(inLayout);
        return *this;
    }

    PipelineLayoutCreateInfo& PipelineLayoutCreateInfo::AddPipelineConstantLayout(const PipelineConstantLayout& inLayout)
    {
        pipelineConstantLayouts.emplace_back(inLayout);
        return *this;
    }

    PipelineLayout::PipelineLayout(const PipelineLayoutCreateInfo&) {}

    PipelineLayout::~PipelineLayout() = default;
}
