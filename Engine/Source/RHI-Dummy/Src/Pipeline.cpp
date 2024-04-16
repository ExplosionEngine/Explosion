//
// Created by johnk on 2023/3/21.
//

#include <RHI/Dummy/Pipeline.h>

namespace RHI::Dummy {
    DummyComputePipeline::DummyComputePipeline(const ComputePipelineCreateInfo& createInfo)
        : ComputePipeline(createInfo)
    {
    }

    DummyComputePipeline::~DummyComputePipeline() = default;

    DummyRasterPipeline::DummyRasterPipeline(const RasterPipelineCreateInfo& createInfo)
        : RasterPipeline(createInfo)
    {
    }

    DummyRasterPipeline::~DummyRasterPipeline() = default;
}
