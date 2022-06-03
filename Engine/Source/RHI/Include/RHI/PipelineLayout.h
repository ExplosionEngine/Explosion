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
        uint32_t bindGroupNum;
        const BindGroupLayout* const* bindGroupLayouts;
        // TODO[DX12] pipeline constant
        uint32_t pipelineConstantNum;
        const PipelineConstantLayout* pipelineConstants;
    };

    class PipelineLayout {
    public:
        NON_COPYABLE(PipelineLayout)
        virtual ~PipelineLayout();

        virtual void Destroy() = 0;

    protected:
        explicit PipelineLayout(const PipelineLayoutCreateInfo* createInfo);
    };
}
