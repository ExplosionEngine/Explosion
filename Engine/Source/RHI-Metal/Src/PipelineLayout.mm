//
// Created by Zach Lee on 2022/10/28.
//

#include <Metal/PipelineLayout.h>
#include <Metal/Device.h>

namespace RHI::Metal {

    MTLPipelineLayout::MTLPipelineLayout(MTLDevice& dev, const PipelineLayoutCreateInfo& createInfo)
        : PipelineLayout(createInfo), device(dev)
    {
    }

    MTLPipelineLayout::~MTLPipelineLayout()
    {
    }

    void MTLPipelineLayout::Destroy()
    {
        delete this;
    }
}
