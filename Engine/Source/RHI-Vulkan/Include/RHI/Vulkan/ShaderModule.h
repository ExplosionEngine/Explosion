//
// Created by Zach Lee on 2022/4/2.
//

#pragma once

#include <RHI/ShaderModule.h>
#include <vulkan/vulkan.h>
#include <string>
#include <unordered_map>

namespace RHI::Vulkan {
    class VulkanDevice;

    class VulkanShaderModule : public ShaderModule {
    public:
        using ShaderInputLocationTable = std::unordered_map<std::string, uint32_t>;

        NonCopyable(VulkanShaderModule)
        VulkanShaderModule(VulkanDevice& inDevice, const ShaderModuleCreateInfo& inCreateInfo);
        ~VulkanShaderModule() override;

        void Destroy() override;

        VkShaderModule GetNative() const;

        void BuildReflection(const ShaderModuleCreateInfo& createInfo);
        const ShaderInputLocationTable& GetLocationTable() const;

    private:
        void CreateNativeShaderModule(const ShaderModuleCreateInfo& createInfo);

        VulkanDevice& device;
        VkShaderModule nativeShaderModule;
        std::unordered_map<std::string, uint32_t> inputLocationTable;
    };

}