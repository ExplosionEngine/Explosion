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

    void DummyComputePipeline::Destroy()
    {
        delete this;
    }

    DummyGraphicsPipeline::DummyGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo)
        : GraphicsPipeline(createInfo)
    {
    }

    DummyGraphicsPipeline::~DummyGraphicsPipeline() = default;

    void DummyGraphicsPipeline::Destroy()
    {
        delete this;
    }
}
