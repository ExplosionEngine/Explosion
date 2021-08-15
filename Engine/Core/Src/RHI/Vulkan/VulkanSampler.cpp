//
// Created by Administrator on 2021/5/29 0029.
//

#include <stdexcept>

#include <Engine/RHI/Vulkan/VulkanSampler.h>
#include <Engine/RHI/Vulkan/VulkanDriver.h>
#include <Engine/RHI/Vulkan/VulkanDevice.h>
#include <Engine/RHI/Vulkan/VulkanAdapater.h>

namespace Explosion::RHI {
    VulkanSampler::VulkanSampler(VulkanDriver& driver, Config config)
        : Sampler(config), driver(driver), device(*driver.GetDevice())
    {
        CreateSampler();
    }

    VulkanSampler::~VulkanSampler()
    {
        DestroySampler();
    }

    const VkSampler& VulkanSampler::GetVkSampler()
    {
        return vkSampler;
    }

    void VulkanSampler::CreateSampler()
    {
        VkSamplerCreateInfo createInfo {};
        createInfo.magFilter = VkConvert<SamplerFilter, VkFilter>(config.magFilter);
        createInfo.minFilter = VkConvert<SamplerFilter, VkFilter>(config.minFilter);
        createInfo.addressModeU = VkConvert<SamplerAddressMode, VkSamplerAddressMode>(config.addressModeU);
        createInfo.addressModeV = VkConvert<SamplerAddressMode, VkSamplerAddressMode>(config.addressModeV);
        createInfo.addressModeW = VkConvert<SamplerAddressMode, VkSamplerAddressMode>(config.addressModeW);
        createInfo.anisotropyEnable = VkBoolConvert(config.anisotropyEnabled);
        createInfo.maxAnisotropy = config.maxAnisotropy;
        createInfo.borderColor = VkConvert<BorderColor, VkBorderColor>(config.borderColor);
        createInfo.unnormalizedCoordinates = VkBoolConvert(config.unNormalizedCoordinates);
        createInfo.compareEnable = VkBoolConvert(config.compareEnabled);
        createInfo.compareOp = VkConvert<CompareOp, VkCompareOp>(config.compareOp);
        createInfo.mipmapMode = VkConvert<SamplerMipmapMode, VkSamplerMipmapMode>(config.mipmapMode);
        createInfo.mipLodBias = config.mipLodBias;
        createInfo.minLod = config.minLod;
        createInfo.maxLod = config.maxLod;

        if (vkCreateSampler(device.GetVkDevice(), &createInfo, nullptr, &vkSampler) != VK_SUCCESS) {
            throw std::runtime_error("failed to create sampler");
        }
    }

    void VulkanSampler::DestroySampler()
    {
        vkDestroySampler(device.GetVkDevice(), vkSampler, nullptr);
    }
}
