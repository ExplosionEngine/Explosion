//
// Created by johnk on 8/1/2022.
//

#ifndef EXPLOSION_RHI_PIPELINE_LAYOUT_H
#define EXPLOSION_RHI_PIPELINE_LAYOUT_H

#include <Common/Utility.h>

namespace RHI {
    struct DescriptorSetLayout;

    struct PipelineLayoutCreateInfo {
        size_t descriptorSetLayoutNum;
        const DescriptorSetLayout* descriptorSetLayouts;
    };

    class PipelineLayout {
    public:
        NON_COPYABLE(PipelineLayout)
        virtual ~PipelineLayout();

    protected:
        explicit PipelineLayout(const PipelineLayoutCreateInfo* createInfo);
    };
}

#endif //EXPLOSION_RHI_PIPELINE_LAYOUT_H
