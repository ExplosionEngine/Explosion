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
        PipelineConstantLayout& SetStageFlags(ShaderStageFlags inStageFlags);
        PipelineConstantLayout& SetOffset(uint32_t inOffset);
        PipelineConstantLayout& SetSize(uint32_t inSize);
    };

    struct PipelineLayoutCreateInfo {
        std::vector<const BindGroupLayout*> bindGroupLayouts;
        std::vector<PipelineConstantLayout> pipelineConstantLayouts;
        std::string debugName;

        PipelineLayoutCreateInfo();
        PipelineLayoutCreateInfo& AddBindGroupLayout(const BindGroupLayout* inLayout);
        PipelineLayoutCreateInfo& AddPipelineConstantLayout(const PipelineConstantLayout& inLayout);
    };

    class PipelineLayout {
    public:
        NonCopyable(PipelineLayout)
        virtual ~PipelineLayout();

    protected:
        explicit PipelineLayout(const PipelineLayoutCreateInfo& createInfo);
    };
}
