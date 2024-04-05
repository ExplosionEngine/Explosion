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
        CompareFunc comparisonFunc;
        uint8_t maxAnisotropy;
        std::string debugName;

        SamplerCreateInfo();
        SamplerCreateInfo& SetAddressModeU(AddressMode inMode);
        SamplerCreateInfo& SetAddressModeV(AddressMode inMode);
        SamplerCreateInfo& SetAddressModeW(AddressMode inMode);
        SamplerCreateInfo& SetMagFilter(FilterMode inMode);
        SamplerCreateInfo& SetMinFilter(FilterMode inMode);
        SamplerCreateInfo& SetMipFilter(FilterMode inMode);
        SamplerCreateInfo& SetLodMinClamp(float inValue);
        SamplerCreateInfo& SetLodMaxClamp(float inValue);
        SamplerCreateInfo& SetComparisonFunc(CompareFunc inFunc);
        SamplerCreateInfo& SetMaxAnisotropy(uint8_t inValue);
        SamplerCreateInfo& SetDebugName(std::string inName);
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
