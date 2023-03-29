//
// Created by johnk on 2023/3/21.
//

#pragma once

#include <RHI/Sampler.h>

namespace RHI::Dummy {
    class DummySampler : public Sampler {
    public:
        explicit DummySampler(const SamplerCreateInfo& createInfo);
        ~DummySampler() override;

        void Destroy() override;
    };
}
