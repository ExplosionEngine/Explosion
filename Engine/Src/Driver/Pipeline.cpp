//
// Created by John Kindem on 2021/3/30.
//

#include <utility>

#include <Explosion/Driver/Pipeline.h>
#include <Explosion/Driver/Device.h>

namespace {
    VkShaderModule CreateShaderModule(const VkDevice& device, const std::vector<char>& code)
    {
        VkShaderModuleCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shaderModule {};
        if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module");
        }
        return shaderModule;
    }

    void DestroyShaderModule(const VkDevice& device, const VkShaderModule& shaderModule)
    {
        vkDestroyShaderModule(device, shaderModule, nullptr);
    }
}

namespace Explosion {
    Pipeline::Pipeline(Device& device, Pipeline::Config config) : device(device), config(std::move(config)) {}

    Pipeline::~Pipeline() = default;

    void Pipeline::CreatePipeline()
    {
        VkShaderModule vertexShaderModule = CreateShaderModule(device.GetVkDevice(), config.shaderConfig.vertexShaderCode);
        VkShaderModule fragmentShaderModule = CreateShaderModule(device.GetVkDevice(), config.shaderConfig.fragmentShaderCode);
        std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos(2);
        shaderStageCreateInfos[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStageCreateInfos[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStageCreateInfos[0].module = vertexShaderModule;
        shaderStageCreateInfos[0].pName = "VertexShader";
        shaderStageCreateInfos[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStageCreateInfos[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStageCreateInfos[1].module = fragmentShaderModule;
        shaderStageCreateInfos[1].pName = "FragmentShader";

        // TODO

        DestroyShaderModule(device.GetVkDevice(), vertexShaderModule);
        DestroyShaderModule(device.GetVkDevice(), fragmentShaderModule);
    }

    void Pipeline::DestroyPipeline()
    {
        // TODO
    }
}
