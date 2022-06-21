//
// Created by Zach Lee on 2022/4/2.
//

#include <RHI/Vulkan/ShaderModule.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Common.h>
#include <spirv_cross/spirv_cross.hpp>

namespace RHI::Vulkan {

    VKShaderModule::VKShaderModule(VKDevice& dev, const ShaderModuleCreateInfo* createInfo)
        : device(dev), ShaderModule(createInfo)
    {
        CreateNativeShaderModule(createInfo);
    }

    VKShaderModule::~VKShaderModule()
    {
        if (shaderModule) {
            device.GetVkDevice().destroyShaderModule(shaderModule, nullptr);
        }
    }

    void VKShaderModule::Destroy()
    {
        delete this;
    }

    vk::ShaderModule VKShaderModule::GetNativeHandle() const
    {
        return shaderModule;
    }

    void VKShaderModule::CreateNativeShaderModule(const ShaderModuleCreateInfo* createInfo)
    {
        vk::ShaderModuleCreateInfo moduleCreateInfo = {};
        moduleCreateInfo.setCodeSize(createInfo->size)
            .setPCode(static_cast<const uint32_t*>(createInfo->byteCode));

        Assert(device.GetVkDevice().createShaderModule(&moduleCreateInfo, nullptr, &shaderModule) == vk::Result::eSuccess);
        BuildReflection(createInfo);
    }

    void VKShaderModule::BuildReflection(const ShaderModuleCreateInfo* createInfo)
    {
        spirv_cross::Compiler compiler(static_cast<const uint32_t*>(createInfo->byteCode),
                                       createInfo->size / sizeof(uint32_t));
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