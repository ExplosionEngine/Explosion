//
// Created by johnk on 11/1/2022.
//

#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include <RHI/Instance.h>
#include <RHI/Vulkan/Api.h>

namespace RHI::Vulkan {
    RHI::Instance* RHIGetInstance();

    class VKInstance : public Instance {
    public:
        NON_COPYABLE(VKInstance)
        VKInstance();
        ~VKInstance() override;

        RHIType GetRHIType() override;
        uint32_t GetGpuNum() override;
        Gpu* GetGpu(uint32_t index) override;
        VkInstance GetVkInstance() const;
        void Destroy() override;

        // function pointer
#if BUILD_CONFIG_DEBUG
        PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
        PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;
        PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT;
#endif
        PFN_vkCmdBeginRenderingKHR vkCmdBeginRenderingKHR;
        PFN_vkCmdEndRenderingKHR  vkCmdEndRenderingKHR;

    private:
        void PrepareExtensions();
        void CreateVKInstance();
        void DestroyVKInstance();
        void PreparePFN();
        void EnumeratePhysicalDevices();
#if BUILD_CONFIG_DEBUG
        void PrepareLayers();
        void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        void CreateDebugMessenger();
        void DestroyDebugMessenger();
#endif

        VkInstance vkInstance;
        std::vector<const char*> vkEnabledExtensionNames;
        std::vector<VkPhysicalDevice> vkPhysicalDevices;
        std::vector<Common::UniqueRef<Gpu>> gpus;
#if BUILD_CONFIG_DEBUG
        std::vector<const char*> vkEnabledLayerNames;
        VkDebugUtilsMessengerEXT vkDebugMessenger;
#endif
    };
}
