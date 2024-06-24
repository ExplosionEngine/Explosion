//
// Created by Zach Lee on 2022/4/2.
//

#pragma once

#include <string>

#include <vulkan/vulkan.h>

#include <RHI/ShaderModule.h>

namespace RHI::Vulkan {
    class VulkanDevice;

    class VulkanShaderModule final : public ShaderModule {
    public:
        NonCopyable(VulkanShaderModule)
        VulkanShaderModule(VulkanDevice& inDevice, const ShaderModuleCreateInfo& inCreateInfo);
        ~VulkanShaderModule() override;

        const std::string& GetEntryPoint() override;

        VkShaderModule GetNative() const;

    private:
        void CreateNativeShaderModule(const ShaderModuleCreateInfo& createInfo);

        VulkanDevice& device;
        VkShaderModule nativeShaderModule;
        std::string entryPoint;
    };
}