//
// Created by Zach Lee on 2022/4/2.
//

#include <RHI/Vulkan/ShaderModule.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Common.h>

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

        if (device.GetVkDevice().createShaderModule(&moduleCreateInfo, nullptr, &shaderModule) != vk::Result::eSuccess) {
            throw VKException("failed to create shader module");
        }
    }
}