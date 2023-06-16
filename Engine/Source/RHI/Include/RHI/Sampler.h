//
// Created by johnk on 19/2/2022.
//

#pragma once

#include <Common/Utility.h>
#include <RHI/Common.h>
#include <string>

namespace RHI {
    struct SamplerCreateInfo {
        AddressMode addressModeU = AddressMode::clampToEdge;
        AddressMode addressModeV = AddressMode::clampToEdge;
        AddressMode addressModeW = AddressMode::clampToEdge;
        FilterMode magFilter = FilterMode::nearest;
        FilterMode minFilter = FilterMode::nearest;
        FilterMode mipFilter = FilterMode::nearest;
        float lodMinClamp = 0;
        float lodMaxClamp = 32;
        ComparisonFunc comparisonFunc = ComparisonFunc::never;
        uint8_t maxAnisotropy = 1;
        std::string debugName;
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
