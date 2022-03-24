//
// Created by johnk on 19/2/2022.
//

#pragma once

#include <cstdint>
#include <Common/Utility.h>

namespace RHI {
    class BindGroupLayout;

    struct PipelineLayoutCreateInfo {
        uint32_t bindGroupNum;
        const BindGroupLayout* bindGroupLayouts;
        // TODO pipeline constant
        // uint32 pipelineConstantNum;
        // const PipelineConstantLayout* pipelineConstants;
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
