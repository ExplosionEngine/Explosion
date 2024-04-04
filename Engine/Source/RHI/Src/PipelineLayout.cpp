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

    PipelineConstantLayout::PipelineConstantLayout(ShaderStageFlags inStageFlags, uint32_t inOffset, uint32_t inSize)
        : stageFlags(inStageFlags)
        , offset(inOffset)
        , size(inSize)
    {
    }

    PipelineConstantLayout& PipelineConstantLayout::SetStageFlags(ShaderStageFlags inStageFlags)
    {
        stageFlags = inStageFlags;
        return *this;
    }

    PipelineConstantLayout& PipelineConstantLayout::SetOffset(uint32_t inOffset)
    {
        offset = inOffset;
        return *this;
    }

    PipelineConstantLayout& PipelineConstantLayout::SetSize(uint32_t inSize)
    {
        size = inSize;
        return *this;
    }

    PipelineLayoutCreateInfo::PipelineLayoutCreateInfo()
        : bindGroupLayouts()
        , pipelineConstantLayouts()
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

    PipelineLayout::PipelineLayout(const PipelineLayoutCreateInfo& createInfo) {}

    PipelineLayout::~PipelineLayout() = default;
}
