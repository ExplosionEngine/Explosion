//
// Created by Zach Lee on 2021/9/25.
//

#include <RHI/Vulkan/VulkanDriver.h>
#include <RHI/Vulkan/VulkanShader.h>
#include <RHI/Vulkan/VulkanAdapater.h>

namespace Explosion::RHI {

    VulkanShader::VulkanShader(VulkanDriver& driver, Config config)
        : Shader(config), driver(driver), device(*driver.GetDevice()), shaderModule(VK_NULL_HANDLE)
    {
        CreateShader();
    }

    VulkanShader::~VulkanShader()
    {
        DestroyShader();
    }

    const VkShaderModule& VulkanShader::GetShaderModule() const
    {
        return shaderModule;
    }

    VkShaderStageFlagBits VulkanShader::GetShaderStage() const
    {
        return VkConvert<ShaderStageBits, VkShaderStageFlagBits>(config.stage);
    }

    void VulkanShader::CreateShader()
    {
        VkShaderModuleCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.codeSize = config.data.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(config.data.data());

        if (vkCreateShaderModule(device.GetVkDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vulkan shader module");
        }
    }

    void VulkanShader::DestroyShader()
    {
        vkDestroyShaderModule(device.GetVkDevice(), shaderModule, nullptr);
    }

}
