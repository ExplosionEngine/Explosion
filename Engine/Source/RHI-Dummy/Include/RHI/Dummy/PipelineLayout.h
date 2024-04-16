//
// Created by johnk on 2023/3/21.
//

#pragma once

#include <RHI/PipelineLayout.h>

namespace RHI::Dummy {
    class DummyPipelineLayout : public PipelineLayout {
    public:
        NonCopyable(DummyPipelineLayout)
        explicit DummyPipelineLayout(const PipelineLayoutCreateInfo& createInfo);
        ~DummyPipelineLayout() override;
    };
}
