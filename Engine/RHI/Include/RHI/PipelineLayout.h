//
// Created by johnk on 19/2/2022.
//

#ifndef EXPLOSION_RHI_PIPELINE_LAYOUT_H
#define EXPLOSION_RHI_PIPELINE_LAYOUT_H

#include <cstdint>
#include <Common/Utility.h>

namespace RHI {
    class BindGroupLayout;

    struct PipelineLayoutCreateInfo {
        uint32_t bindGroupNum;
        const BindGroupLayout* bindGroupLayouts;
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

#endif//EXPLOSION_RHI_PIPELINE_LAYOUT_H
