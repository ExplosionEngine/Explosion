//
// Created by Zach Lee on 2022/3/6.
//

#include <RHI/Vulkan/Sampler.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Common.h>

namespace RHI::Vulkan {

    static vk::SamplerAddressMode GetVKAddressMode(AddressMode type)
    {
        static std::unordered_map<AddressMode, vk::SamplerAddressMode> modes = {
            { AddressMode::CLAMP_TO_EDGE,  vk::SamplerAddressMode::eClampToEdge },
            { AddressMode::REPEAT,         vk::SamplerAddressMode::eRepeat },
            { AddressMode::MIRROR_REPEAT,  vk::SamplerAddressMode::eMirroredRepeat }
        };

        auto iter = modes.find(type);
        if (iter == modes.end()) {
            throw VKException("AddressMode not supported.");
        }
        return iter->second;
    }

    static vk::Filter GetVKFilterMode(FilterMode type)
    {
        static std::unordered_map<FilterMode, vk::Filter> modes = {
            { FilterMode::NEAREST,  vk::Filter::eNearest },
            { FilterMode::LINEAR,   vk::Filter::eLinear },
        };

        auto iter = modes.find(type);
        if (iter == modes.end()) {
            throw VKException("FilterMode not supported.");
        }
        return iter->second;
    }

    static vk::SamplerMipmapMode GetVKMipmapMode(FilterMode type)
    {
        static std::unordered_map<FilterMode, vk::SamplerMipmapMode> modes = {
            { FilterMode::NEAREST,  vk::SamplerMipmapMode::eNearest },
            { FilterMode::LINEAR,   vk::SamplerMipmapMode::eLinear },
        };

        auto iter = modes.find(type);
        if (iter == modes.end()) {
            throw VKException("FilterMode not supported.");
        }
        return iter->second;
    }

    static vk::CompareOp GetCompareOp(ComparisonFunc type)
    {
        static std::unordered_map<ComparisonFunc, vk::CompareOp> compareOps = {
            { ComparisonFunc::NEVER,         vk::CompareOp::eNever },
            { ComparisonFunc::LESS,          vk::CompareOp::eLess },
            { ComparisonFunc::EQUAL,         vk::CompareOp::eEqual },
            { ComparisonFunc::LESS_EQUAL,    vk::CompareOp::eLessOrEqual },
            { ComparisonFunc::GREATER,       vk::CompareOp::eGreater },
            { ComparisonFunc::NOT_EQUAL,     vk::CompareOp::eNotEqual },
            { ComparisonFunc::GREATER_EQUAL, vk::CompareOp::eGreaterOrEqual },
            { ComparisonFunc::ALWAYS,        vk::CompareOp::eAlways },
        };

        auto iter = compareOps.find(type);
        if (iter == compareOps.end()) {
            throw VKException("ComparisonFunc not supported.");
        }
        return iter->second;
    }

    VKSampler::VKSampler(VKDevice& dev, const SamplerCreateInfo* createInfo)
        : Sampler(createInfo), device(dev), vkSampler(VK_NULL_HANDLE)
    {
        CreateSampler(createInfo);
    }

    VKSampler::~VKSampler()
    {
        if (vkSampler) {
            device.GetVkDevice().destroySampler(vkSampler, nullptr);
        }
    }

    void VKSampler::Destroy()
    {
        delete this;
    }

    void VKSampler::CreateSampler(const SamplerCreateInfo* createInfo)
    {
        vk::SamplerCreateInfo samplerInfo = {};

        samplerInfo.setAddressModeU(GetVKAddressMode(createInfo->addressModeU))
            .setAddressModeV(GetVKAddressMode(createInfo->addressModeV))
            .setAddressModeW(GetVKAddressMode(createInfo->addressModeW))
            .setMinFilter(GetVKFilterMode(createInfo->minFilter))
            .setMagFilter(GetVKFilterMode(createInfo->magFilter))
            .setMipmapMode(GetVKMipmapMode(createInfo->mipFilter))
            .setMinLod(createInfo->lodMinClamp)
            .setMaxLod(createInfo->lodMaxClamp)
            .setCompareEnable(createInfo->comparisonFunc != ComparisonFunc::NEVER)
            .setCompareOp(GetCompareOp(createInfo->comparisonFunc))
            .setAnisotropyEnable(createInfo->maxAnisotropy > 1)
            .setMaxAnisotropy(createInfo->maxAnisotropy);

        if (device.GetVkDevice().createSampler(&samplerInfo, nullptr, &vkSampler) != vk::Result::eSuccess) {
            throw VKException("failed to create sampler");
        }
    }
}