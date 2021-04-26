//
// Created by John Kindem on 2021/4/26.
//

#include <Explosion/Driver/Pipeline.h>
#include <Explosion/Driver/Driver.h>

namespace Explosion {
    Pipeline::Pipeline(Driver& driver)
        : driver(driver), device(*driver.GetDevice()) {}

    Pipeline::~Pipeline() = default;

    VkShaderModule Pipeline::CreateShaderModule(const std::vector<char>& code)
    {
        VkShaderModuleCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shaderModule = VK_NULL_HANDLE;
        if (vkCreateShaderModule(device.GetVkDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vulkan shader module");
        }
        return shaderModule;
    }

    void Pipeline::DestroyShaderModule(const VkShaderModule& shaderModule)
    {
        vkDestroyShaderModule(device.GetVkDevice(), shaderModule, nullptr);
    }

    GraphicsPipeline::GraphicsPipeline(Driver& driver, const GraphicsPipeline::Config& config)
        : Pipeline(driver), config(config)
    {
        CreateGraphicsPipeline();
    }

    GraphicsPipeline::~GraphicsPipeline()
    {
        DestroyGraphicsPipeline();
    }

    void GraphicsPipeline::CreateGraphicsPipeline()
    {
        // TODO
    }

    void GraphicsPipeline::DestroyGraphicsPipeline()
    {
        // TODO
    }
}
