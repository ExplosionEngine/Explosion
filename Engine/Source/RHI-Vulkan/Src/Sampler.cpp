//
// Created by Zach Lee on 2022/3/6.
//

#include <RHI/Vulkan/Sampler.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Common.h>

namespace RHI::Vulkan {
    VulkanSampler::VulkanSampler(VulkanDevice& inDevice, const SamplerCreateInfo& inCreateInfo)
        : Sampler(inCreateInfo), device(inDevice), nativeSampler(VK_NULL_HANDLE)
    {
        CreateSampler(inCreateInfo);
    }

    VulkanSampler::~VulkanSampler()
    {
        if (nativeSampler != VK_NULL_HANDLE) {
            vkDestroySampler(device.GetNative(), nativeSampler, nullptr);
        }
    }

    void VulkanSampler::Destroy()
    {
        delete this;
    }

    VkSampler VulkanSampler::GetNative() const
    {
        return nativeSampler;
    }

    void VulkanSampler::CreateSampler(const SamplerCreateInfo& inCreateInfo)
    {
        VkSamplerCreateInfo samplerInfo = {};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.addressModeU = VKEnumCast<AddressMode, VkSamplerAddressMode>(inCreateInfo.addressModeU);
        samplerInfo.addressModeV = VKEnumCast<AddressMode, VkSamplerAddressMode>(inCreateInfo.addressModeV);
        samplerInfo.addressModeW = VKEnumCast<AddressMode, VkSamplerAddressMode>(inCreateInfo.addressModeW);
        samplerInfo.minFilter = VKEnumCast<FilterMode, VkFilter>(inCreateInfo.minFilter);
        samplerInfo.magFilter = VKEnumCast<FilterMode, VkFilter>(inCreateInfo.magFilter);
        samplerInfo.mipmapMode = VKEnumCast<FilterMode, VkSamplerMipmapMode>(inCreateInfo.mipFilter);
        samplerInfo.minLod = inCreateInfo.lodMinClamp;
        samplerInfo.maxLod = inCreateInfo.lodMaxClamp;
        samplerInfo.compareEnable = inCreateInfo.comparisonFunc != CompareFunc::never;
        samplerInfo.compareOp = VKEnumCast<CompareFunc, VkCompareOp>(inCreateInfo.comparisonFunc);
        samplerInfo.anisotropyEnable = inCreateInfo.maxAnisotropy > 1;
        samplerInfo.maxAnisotropy = inCreateInfo.maxAnisotropy;

        Assert(vkCreateSampler(device.GetNative(), &samplerInfo, nullptr, &nativeSampler) == VK_SUCCESS);

#if BUILD_CONFIG_DEBUG
        if (!inCreateInfo.debugName.empty()) {
            device.SetObjectName(VK_OBJECT_TYPE_SAMPLER, reinterpret_cast<uint64_t>(nativeSampler), inCreateInfo.debugName.c_str());
        }
#endif
    }
}