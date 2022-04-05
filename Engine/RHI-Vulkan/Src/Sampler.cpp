//
// Created by Zach Lee on 2022/3/6.
//

#include <RHI/Vulkan/Sampler.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Common.h>

namespace RHI::Vulkan {
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

        samplerInfo.setAddressModeU(VKEnumCast<AddressMode, vk::SamplerAddressMode>(createInfo->addressModeU))
            .setAddressModeV(VKEnumCast<AddressMode, vk::SamplerAddressMode>(createInfo->addressModeV))
            .setAddressModeW(VKEnumCast<AddressMode, vk::SamplerAddressMode>(createInfo->addressModeW))
            .setMinFilter(VKEnumCast<FilterMode, vk::Filter>(createInfo->minFilter))
            .setMagFilter(VKEnumCast<FilterMode, vk::Filter>(createInfo->magFilter))
            .setMipmapMode(VKEnumCast<FilterMode, vk::SamplerMipmapMode>(createInfo->mipFilter))
            .setMinLod(createInfo->lodMinClamp)
            .setMaxLod(createInfo->lodMaxClamp)
            .setCompareEnable(createInfo->comparisonFunc != ComparisonFunc::NEVER)
            .setCompareOp(VKEnumCast<ComparisonFunc, vk::CompareOp>(createInfo->comparisonFunc))
            .setAnisotropyEnable(createInfo->maxAnisotropy > 1)
            .setMaxAnisotropy(createInfo->maxAnisotropy);

        if (device.GetVkDevice().createSampler(&samplerInfo, nullptr, &vkSampler) != vk::Result::eSuccess) {
            throw VKException("failed to create renderPass");
        }
    }
}