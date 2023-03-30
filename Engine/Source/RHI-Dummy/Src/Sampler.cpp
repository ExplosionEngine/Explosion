//
// Created by johnk on 2023/3/21.
//

#include <RHI/Dummy/Sampler.h>

namespace RHI::Dummy {
    DummySampler::DummySampler(const SamplerCreateInfo& createInfo)
        : Sampler(createInfo)
    {
    }

    DummySampler::~DummySampler() = default;

    void DummySampler::Destroy()
    {
        delete this;
    }
}
