//
// Created by Zach Lee on 2022/3/6.
//

#include <RHI/Vulkan/Sampler.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Common.h>

namespace RHI::Vulkan {
    VKSampler::VKSampler(VKDevice& dev, const SamplerCreateInfo& createInfo)
        : Sampler(createInfo), device(dev), vkSampler(VK_NULL_HANDLE)
    {
        CreateSampler(createInfo);
    }

    VKSampler::~VKSampler()
    {
        if (vkSampler) {
            vkDestroySampler(device.GetVkDevice(), vkSampler, nullptr);
        }
    }

    void VKSampler::Destroy()
    {
        delete this;
    }

    VkSampler VKSampler::GetVkSampler() const
    {
        return vkSampler;
    }

    void VKSampler::CreateSampler(const SamplerCreateInfo& createInfo)
    {
        VkSamplerCreateInfo samplerInfo = {};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.addressModeU = VKEnumCast<AddressMode, VkSamplerAddressMode>(createInfo.addressModeU);
        samplerInfo.addressModeV = VKEnumCast<AddressMode, VkSamplerAddressMode>(createInfo.addressModeV);
        samplerInfo.addressModeW = VKEnumCast<AddressMode, VkSamplerAddressMode>(createInfo.addressModeW);
        samplerInfo.minFilter = VKEnumCast<FilterMode, VkFilter>(createInfo.minFilter);
        samplerInfo.magFilter = VKEnumCast<FilterMode, VkFilter>(createInfo.magFilter);
        samplerInfo.mipmapMode = VKEnumCast<FilterMode, VkSamplerMipmapMode>(createInfo.mipFilter);
        samplerInfo.minLod = createInfo.lodMinClamp;
        samplerInfo.maxLod = createInfo.lodMaxClamp;
        samplerInfo.compareEnable = createInfo.comparisonFunc != ComparisonFunc::never;
        samplerInfo.compareOp = VKEnumCast<ComparisonFunc, VkCompareOp>(createInfo.comparisonFunc);
        samplerInfo.anisotropyEnable = createInfo.maxAnisotropy > 1;
        samplerInfo.maxAnisotropy = createInfo.maxAnisotropy;

        Assert(vkCreateSampler(device.GetVkDevice(), &samplerInfo, nullptr, &vkSampler) == VK_SUCCESS);

#if BUILD_CONFIG_DEBUG
        if (!createInfo.debugName.empty()) {
            device.SetObjectName(VK_OBJECT_TYPE_SAMPLER, reinterpret_cast<uint64_t>(vkSampler), createInfo.debugName.c_str());
        }
#endif
    }
}