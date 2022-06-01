//
// Created by johnk on 19/2/2022.
//

#include <RHI/Pipeline.h>

namespace RHI {
    Pipeline::Pipeline() = default;

    Pipeline::~Pipeline() = default;

    ComputePipeline::ComputePipeline(const ComputePipelineCreateInfo* createInfo) {}

    ComputePipeline::~ComputePipeline() = default;

    GraphicsPipeline::GraphicsPipeline(const GraphicsPipelineCreateInfo* createInfo) {}

    GraphicsPipeline::~GraphicsPipeline() = default;
}
