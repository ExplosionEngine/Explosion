//
// Created by Zach Lee on 2022/4/2.
//

#include <RHI/Vulkan/ShaderModule.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Common.h>
#include <spirv_cross/spirv_cross.hpp>

namespace RHI::Vulkan {

    VKShaderModule::VKShaderModule(VKDevice& dev, const ShaderModuleCreateInfo& createInfo)
        : device(dev), ShaderModule(createInfo)
    {
        CreateNativeShaderModule(createInfo);
    }

    VKShaderModule::~VKShaderModule()
    {
        if (shaderModule) {
            vkDestroyShaderModule(device.GetVkDevice(), shaderModule, nullptr);
        }
    }

    void VKShaderModule::Destroy()
    {
        delete this;
    }

    VkShaderModule VKShaderModule::GetVkShaderModule() const
    {
        return shaderModule;
    }

    void VKShaderModule::CreateNativeShaderModule(const ShaderModuleCreateInfo& createInfo)
    {
        VkShaderModuleCreateInfo moduleCreateInfo = {};
        moduleCreateInfo.codeSize = createInfo.size;
        moduleCreateInfo.pCode = static_cast<const uint32_t*>(createInfo.byteCode);

        Assert(vkCreateShaderModule(device.GetVkDevice(), &moduleCreateInfo, nullptr, &shaderModule) == VK_SUCCESS);
        BuildReflection(createInfo);
    }

    void VKShaderModule::BuildReflection(const ShaderModuleCreateInfo& createInfo)
    {
        // TODO move to render module
        spirv_cross::Compiler compiler(static_cast<const uint32_t*>(createInfo.byteCode),
                                       createInfo.size / sizeof(uint32_t));
        auto resources = compiler.get_shader_resources();
        for (auto& input : resources.stage_inputs) {
            auto location = compiler.get_decoration(input.id, spv::DecorationLocation);
            inputLocationTable.emplace(input.name, location);
        }
    }

    const VKShaderModule::ShaderInputLocationTable& VKShaderModule::GetLocationTable() const
    {
        return inputLocationTable;
    }
}