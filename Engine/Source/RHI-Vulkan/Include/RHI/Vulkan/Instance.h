//
// Created by johnk on 11/1/2022.
//

#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include <RHI/Instance.h>
#include <RHI/Vulkan/Api.h>

namespace RHI::Vulkan {
    extern Instance* gInstance;

    class VulkanInstance : public Instance {
    public:
        NonCopyable(VulkanInstance)
        VulkanInstance();
        ~VulkanInstance() override;

        RHIType GetRHIType() override;
        uint32_t GetGpuNum() override;
        Gpu* GetGpu(uint32_t inIndex) override;
        void Destroy() override;

        VkInstance GetNative() const;

        // function pointer
#if BUILD_CONFIG_DEBUG
        PFN_vkCreateDebugUtilsMessengerEXT pfnVkCreateDebugUtilsMessengerEXT;
        PFN_vkDestroyDebugUtilsMessengerEXT pfnVkDestroyDebugUtilsMessengerEXT;
        PFN_vkSetDebugUtilsObjectNameEXT pfnVkSetDebugUtilsObjectNameEXT;
#endif
        PFN_vkCmdBeginRenderingKHR pfnVkCmdBeginRenderingKHR;
        PFN_vkCmdEndRenderingKHR  pfnVkCmdEndRenderingKHR;

    private:
        void PrepareExtensions();
        void CreateNativeInstance();
        void DestroyNativeInstance();
        void PrepareDynamicFuncPointers();
        void EnumeratePhysicalDevices();
#if BUILD_CONFIG_DEBUG
        void PrepareLayers();
        void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& inCreateInfo);
        void CreateDebugMessenger();
        void DestroyDebugMessenger();
#endif

#if BUILD_CONFIG_DEBUG
        VkDebugUtilsMessengerEXT nativeDebugMessenger;
#endif
        VkInstance nativeInstance;
        std::vector<VkPhysicalDevice> nativePhysicalDevices;
        std::vector<const char*> vkEnabledExtensionNames;
        std::vector<const char*> vkEnabledLayerNames;
        std::vector<Common::UniqueRef<Gpu>> gpus;
    };
}
