//
// Created by johnk on 2023/3/21.
//

#pragma once

#include <RHI/Pipeline.h>

namespace RHI::Dummy {
    class DummyComputePipeline : public ComputePipeline {
    public:
        NonCopyable(DummyComputePipeline)
        explicit DummyComputePipeline(const ComputePipelineCreateInfo& createInfo);
        ~DummyComputePipeline() override;

        void Destroy() override;
    };

    class DummyRasterPipeline : public RasterPipeline {
    public:
        NonCopyable(DummyRasterPipeline)
        explicit DummyRasterPipeline(const RasterPipelineCreateInfo& createInfo);
        ~DummyRasterPipeline() override;

        void Destroy() override;
    };
}
