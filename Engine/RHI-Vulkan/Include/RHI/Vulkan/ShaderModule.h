//
// Created by Zach Lee on 2022/4/2.
//

#pragma once

#include <RHI/ShaderModule.h>
#include <vulkan/vulkan.hpp>

namespace RHI::Vulkan {
    class VKDevice;

    class VKShaderModule : public ShaderModule {
    public:
        NON_COPYABLE(VKShaderModule)
        VKShaderModule(VKDevice& device, const ShaderModuleCreateInfo* createInfo);
        ~VKShaderModule() override;

        void Destroy() override;

        vk::ShaderModule GetNativeHandle() const;

    private:
        void CreateNativeShaderModule(const ShaderModuleCreateInfo* createInfo);

        VKDevice& device;
        vk::ShaderModule shaderModule = VK_NULL_HANDLE;
    };

}