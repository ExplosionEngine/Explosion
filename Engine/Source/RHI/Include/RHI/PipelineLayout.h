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
    };

    struct PipelineLayoutCreateInfo {
        uint32_t bindGroupLayoutNum;
        const BindGroupLayout* const* bindGroupLayouts;
        uint32_t pipelineConstantLayoutNum;
        const PipelineConstantLayout* pipelineConstantLayouts;
        std::string debugName;
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
