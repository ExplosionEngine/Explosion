//
// Created by johnk on 11/1/2022.
//

#pragma once

#include <vulkan/vulkan.hpp>

#include <RHI/Instance.h>
#include <RHI/Vulkan/Api.h>

namespace RHI::Vulkan {
    class RHI_VULKAN_API VKInstance : public Instance {
    public:
        NON_COPYABLE(VKInstance)
        VKInstance();
        ~VKInstance() override;

        RHIType GetRHIType() override;
        uint32_t GetGpuNum() override;
        Gpu* GetGpu(uint32_t index) override;

    private:
        void PrepareExtensions();
        void CreateVKInstance();
        void DestroyVKInstance();
        void PrepareDispatch();
        void EnumeratePhysicalDevices();
#if BUILD_CONFIG_DEBUG
        void PrepareLayers();
        void CreateDebugMessenger();
        void DestroyDebugMessenger();
#endif

        vk::Instance vkInstance;
        vk::DispatchLoaderDynamic vkDispatch;
        std::vector<const char*> vkEnabledExtensionNames;
        std::vector<vk::PhysicalDevice> vkPhysicalDevices;
        std::vector<std::unique_ptr<Gpu>> gpus;
#if BUILD_CONFIG_DEBUG
        std::vector<const char*> vkEnabledLayerNames;
        vk::DebugUtilsMessengerEXT vkDebugMessenger;
#endif
    };
}

extern "C" {
    RHI_VULKAN_API RHI::Instance* RHICreateInstance();
}
