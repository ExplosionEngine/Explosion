//
// Created by johnk on 2023/3/21.
//

#pragma once

#include <RHI/Pipeline.h>

namespace RHI::Dummy {
    class DummyComputePipeline : public ComputePipeline {
    public:
        NON_COPYABLE(DummyComputePipeline)
        explicit DummyComputePipeline(const ComputePipelineCreateInfo& createInfo);
        ~DummyComputePipeline() override;

        void Destroy() override;
    };

    class DummyGraphicsPipeline : public GraphicsPipeline {
    public:
        NON_COPYABLE(DummyGraphicsPipeline)
        explicit DummyGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo);
        ~DummyGraphicsPipeline() override;

        void Destroy() override;
    };
}
