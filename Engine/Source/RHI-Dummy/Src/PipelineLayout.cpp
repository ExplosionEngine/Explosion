//
// Created by johnk on 2023/3/21.
//

#include <RHI/Dummy/PipelineLayout.h>

namespace RHI::Dummy {
    DummyPipelineLayout::DummyPipelineLayout(const PipelineLayoutCreateInfo& createInfo)
        : PipelineLayout(createInfo)
    {
    }

    DummyPipelineLayout::~DummyPipelineLayout() = default;
}
