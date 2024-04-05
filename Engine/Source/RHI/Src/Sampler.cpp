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

    SamplerCreateInfo& SamplerCreateInfo::SetAddressModeU(AddressMode inMode)
    {
        addressModeU = inMode;
        return *this;
    }

    SamplerCreateInfo& SamplerCreateInfo::SetAddressModeV(AddressMode inMode)
    {
        addressModeV = inMode;
        return *this;
    }

    SamplerCreateInfo& SamplerCreateInfo::SetAddressModeW(AddressMode inMode)
    {
        addressModeW = inMode;
        return *this;
    }

    SamplerCreateInfo& SamplerCreateInfo::SetMagFilter(FilterMode inMode)
    {
        magFilter = inMode;
        return *this;
    }

    SamplerCreateInfo& SamplerCreateInfo::SetMinFilter(FilterMode inMode)
    {
        minFilter = inMode;
        return *this;
    }

    SamplerCreateInfo& SamplerCreateInfo::SetMipFilter(FilterMode inMode)
    {
        mipFilter = inMode;
        return *this;
    }

    SamplerCreateInfo& SamplerCreateInfo::SetLodMinClamp(float inValue)
    {
        lodMinClamp = inValue;
        return *this;
    }

    SamplerCreateInfo& SamplerCreateInfo::SetLodMaxClamp(float inValue)
    {
        lodMaxClamp = inValue;
        return *this;
    }

    SamplerCreateInfo& SamplerCreateInfo::SetComparisonFunc(enum ComparisonFunc inFunc)
    {
        comparisonFunc = inFunc;
        return *this;
    }

    SamplerCreateInfo& SamplerCreateInfo::SetMaxAnisotropy(uint8_t inValue)
    {
        maxAnisotropy = inValue;
        return *this;
    }

    SamplerCreateInfo& SamplerCreateInfo::SetDebugName(std::string inName)
    {
        debugName = std::move(inName);
        return *this;
    }

    Sampler::Sampler(const SamplerCreateInfo& createInfo) {}

    Sampler::~Sampler() = default;
}
