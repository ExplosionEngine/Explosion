//
// Created by johnk on 19/2/2022.
//

#pragma once

#include <Common/Utility.h>
#include <RHI/Common.h>

namespace RHI {
    struct SamplerCreateInfo {
        AddressMode addressModeU = AddressMode::CLAMP_TO_EDGE;
        AddressMode addressModeV = AddressMode::CLAMP_TO_EDGE;
        AddressMode addressModeW = AddressMode::CLAMP_TO_EDGE;
        FilterMode magFilter = FilterMode::NEAREST;
        FilterMode minFilter = FilterMode::NEAREST;
        FilterMode mipFilter = FilterMode::NEAREST;
        float lodMinClamp = 0;
        float lodMaxClamp = 32;
        ComparisonFunc comparisonFunc = ComparisonFunc::NEVER;
        uint8_t maxAnisotropy = 1;
    };

    class Sampler {
    public:
        NON_COPYABLE(Sampler)
        virtual ~Sampler();

        virtual void Destroy() = 0;

    protected:
        explicit Sampler(const SamplerCreateInfo& createInfo);
    };
}
