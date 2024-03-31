//
// Created by johnk on 19/2/2022.
//

#pragma once

#include <cstdint>
#include <Common/Utility.h>
#include <RHI/RHI.h>

namespace RHI {
    class BindGroupLayout;

    struct PipelineConstantLayout {
        ShaderStageFlags stageFlags;
        uint32_t offset;
        uint32_t size;

        PipelineConstantLayout();
        PipelineConstantLayout(ShaderStageFlags inStageFlags, uint32_t inOffset, uint32_t inSize);
        PipelineConstantLayout& StageFlags(ShaderStageFlags inStageFlags);
        PipelineConstantLayout& Offset(uint32_t inOffset);
        PipelineConstantLayout& Size(uint32_t inSize);
    };

    struct PipelineLayoutCreateInfo {
        std::vector<const BindGroupLayout*> bindGroupLayouts;
        std::vector<PipelineConstantLayout> pipelineConstantLayouts;
        std::string debugName;

        PipelineLayoutCreateInfo();
        PipelineLayoutCreateInfo& BindGroupLayout(const BindGroupLayout* inLayout);
        PipelineLayoutCreateInfo& PipelineConstantLayout(const PipelineConstantLayout& inLayout);
    };

    class PipelineLayout {
    public:
        NonCopyable(PipelineLayout)
        virtual ~PipelineLayout();

        virtual void Destroy() = 0;

    protected:
        explicit PipelineLayout(const PipelineLayoutCreateInfo& createInfo);
    };
}
