//
// Created by johnk on 11/1/2022.
//

#pragma once

#include <vector>
#include <unordered_map>

#include <vulkan/vulkan.h>

#include <RHI/Instance.h>

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

        template <typename T>
        T FindOrGetTypedDynamicFuncPointer(const std::string& inName)
        {
            auto iter = dynamicFuncPointers.find(inName);
            if (iter != dynamicFuncPointers.end()) {
                return static_cast<T>(iter->second);
            }
            dynamicFuncPointers.emplace(std::make_pair(inName, vkGetInstanceProcAddr(nativeInstance, inName.c_str())));
            return static_cast<T>(dynamicFuncPointers.at(inName));
        }

    private:
        void PrepareExtensions();
        void CreateNativeInstance();
        void DestroyNativeInstance();
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
        std::unordered_map<std::string, void*> dynamicFuncPointers;
    };
}
