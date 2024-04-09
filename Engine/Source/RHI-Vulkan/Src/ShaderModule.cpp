//
// Created by Zach Lee on 2022/4/2.
//

#include <spirv_cross/spirv_cross.hpp>

#include <RHI/Vulkan/ShaderModule.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Common.h>

namespace RHI::Vulkan {
    VulkanShaderModule::VulkanShaderModule(VulkanDevice& inDevice, const ShaderModuleCreateInfo& inCreateInfo)
        : ShaderModule(inCreateInfo)
        , device(inDevice)
        , nativeShaderModule(VK_NULL_HANDLE)
        , entryPoint(inCreateInfo.entryPoint)
    {
        CreateNativeShaderModule(inCreateInfo);
    }

    VulkanShaderModule::~VulkanShaderModule()
    {
        if (nativeShaderModule != VK_NULL_HANDLE) {
            vkDestroyShaderModule(device.GetNative(), nativeShaderModule, nullptr);
        }
    }

    const std::string& VulkanShaderModule::GetEntryPoint()
    {
        return entryPoint;
    }

    void VulkanShaderModule::Destroy()
    {
        delete this;
    }

    VkShaderModule VulkanShaderModule::GetNative() const
    {
        return nativeShaderModule;
    }

    void VulkanShaderModule::CreateNativeShaderModule(const ShaderModuleCreateInfo& createInfo)
    {
        VkShaderModuleCreateInfo moduleCreateInfo = {};
        moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        moduleCreateInfo.codeSize = createInfo.size;
        moduleCreateInfo.pCode = static_cast<const uint32_t*>(createInfo.byteCode);

        Assert(vkCreateShaderModule(device.GetNative(), &moduleCreateInfo, nullptr, &nativeShaderModule) == VK_SUCCESS);
    }
}