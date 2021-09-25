//
// Created by Zach Lee on 2021/9/25.
//

#ifndef EXPLOSION_VULKANSHADER_H
#define EXPLOSION_VULKANSHADER_H

#include <vulkan/vulkan.h>

#include <RHI/Shader.h>

namespace Explosion::RHI {

    class VulkanDriver;
    class VulkanDevice;

    class VulkanShader : public Shader {
    public:
        VulkanShader(VulkanDriver& driver, Config config);
        ~VulkanShader() override;

        const VkShaderModule& GetShaderModule() const;

        VkShaderStageFlagBits GetShaderStage() const;

    private:
        void CreateShader();
        void DestroyShader();

        VulkanDriver& driver;
        VulkanDevice& device;
        VkShaderModule shaderModule;
    };

}

#endif//EXPLOSION_VULKANSHADER_H
