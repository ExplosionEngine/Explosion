//
// Created by johnk on 19/2/2022.
//

#include <RHI/Sampler.h>

namespace RHI {
    SamplerCreateInfo::SamplerCreateInfo()
        : addressModeU(AddressMode::clampToEdge)
        , addressModeV(AddressMode::clampToEdge)
        , addressModeW(AddressMode::clampToEdge)
        , magFilter(FilterMode::nearest)
        , minFilter(FilterMode::nearest)
        , mipFilter(FilterMode::nearest)
        , lodMinClamp(0.0f)
        , lodMaxClamp(32.0f)
        , comparisonFunc(ComparisonFunc::never)
        , maxAnisotropy(1)
        , debugName()
    {
    }

    SamplerCreateInfo& SamplerCreateInfo::AddressModeU(AddressMode inMode)
    {
        addressModeU = inMode;
        return *this;
    }

    SamplerCreateInfo& SamplerCreateInfo::AddressModeV(AddressMode inMode)
    {
        addressModeV = inMode;
        return *this;
    }

    SamplerCreateInfo& SamplerCreateInfo::AddressModeW(AddressMode inMode)
    {
        addressModeW = inMode;
        return *this;
    }

    SamplerCreateInfo& SamplerCreateInfo::MagFilter(FilterMode inMode)
    {
        magFilter = inMode;
        return *this;
    }

    SamplerCreateInfo& SamplerCreateInfo::MinFilter(FilterMode inMode)
    {
        minFilter = inMode;
        return *this;
    }

    SamplerCreateInfo& SamplerCreateInfo::LodMinClamp(float inValue)
    {
        lodMinClamp = inValue;
        return *this;
    }

    SamplerCreateInfo& SamplerCreateInfo::LodMaxClamp(float inValue)
    {
        lodMaxClamp = inValue;
        return *this;
    }

    SamplerCreateInfo& SamplerCreateInfo::ComparisonFunc(enum ComparisonFunc inFunc)
    {
        comparisonFunc = inFunc;
        return *this;
    }

    SamplerCreateInfo& SamplerCreateInfo::MaxAnisotropy(uint8_t inValue)
    {
        maxAnisotropy = inValue;
        return *this;
    }

    SamplerCreateInfo& SamplerCreateInfo::DebugName(std::string inName)
    {
        debugName = std::move(inName);
        return *this;
    }

    Sampler::Sampler(const SamplerCreateInfo& createInfo) {}

    Sampler::~Sampler() = default;
}
