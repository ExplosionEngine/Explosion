//
// Created by Zach Lee on 2022/4/2.
//

#pragma once

#include <RHI/ShaderModule.h>
#include <vulkan/vulkan.h>
#include <string>
#include <unordered_map>

namespace RHI::Vulkan {
    class VKDevice;

    class VKShaderModule : public ShaderModule {
    public:
        NonCopyable(VKShaderModule)
        VKShaderModule(VKDevice& device, const ShaderModuleCreateInfo& createInfo);
        ~VKShaderModule() override;

        void Destroy() override;

        VkShaderModule GetVkShaderModule() const;

        void BuildReflection(const ShaderModuleCreateInfo& createInfo);

        using ShaderInputLocationTable = std::unordered_map<std::string, uint32_t>;
        const ShaderInputLocationTable& GetLocationTable() const;

    private:
        void CreateNativeShaderModule(const ShaderModuleCreateInfo& createInfo);

        VKDevice& device;
        VkShaderModule shaderModule = VK_NULL_HANDLE;
        std::unordered_map<std::string, uint32_t> inputLocationTable;
    };

}