//
// Created by johnk on 19/2/2022.
//

#pragma once

#include <Common/Utility.h>
#include <RHI/Common.h>
#include <string>

namespace RHI {
    struct SamplerCreateInfo {
        AddressMode addressModeU;
        AddressMode addressModeV;
        AddressMode addressModeW;
        FilterMode magFilter;
        FilterMode minFilter;
        FilterMode mipFilter;
        float lodMinClamp;
        float lodMaxClamp;
        ComparisonFunc comparisonFunc;
        uint8_t maxAnisotropy;
        std::string debugName;

        SamplerCreateInfo();
        SamplerCreateInfo& AddressModeU(AddressMode inMode);
        SamplerCreateInfo& AddressModeV(AddressMode inMode);
        SamplerCreateInfo& AddressModeW(AddressMode inMode);
        SamplerCreateInfo& MagFilter(FilterMode inMode);
        SamplerCreateInfo& MinFilter(FilterMode inMode);
        SamplerCreateInfo& LodMinClamp(float inValue);
        SamplerCreateInfo& LodMaxClamp(float inValue);
        SamplerCreateInfo& ComparisonFunc(ComparisonFunc inFunc);
        SamplerCreateInfo& MaxAnisotropy(uint8_t inValue);
        SamplerCreateInfo& DebugName(std::string inName);
    };

    class Sampler {
    public:
        NonCopyable(Sampler)
        virtual ~Sampler();

        virtual void Destroy() = 0;

    protected:
        explicit Sampler(const SamplerCreateInfo& createInfo);
    };
}
