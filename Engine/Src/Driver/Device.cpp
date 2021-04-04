//
// Created by John Kindem on 2021/3/30.
//

#include <stdexcept>

#include <Explosion/Driver/Device.h>
#include <Explosion/Driver/Utils.h>

namespace Explosion {
    Device::Device()
    {
        PrepareExtensions();
        CreateInstance();
    }

    Device::~Device()
    {
        DestroyInstance();
    }

    void Device::PrepareExtensions()
    {
#ifdef WIN32
        extensions.emplace_back("VK_KHR_surface");
        extensions.emplace_back("VK_KHR_win32_surface");
#endif

        uint32_t propertiesCnt = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCnt, nullptr);
        std::vector<VkExtensionProperties> properties(propertiesCnt);
        vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCnt, properties.data());
        if (!CheckExtensionSupported(extensions, properties)) {
            throw std::runtime_error("there are some extension is not supported");
        }
    }

    void Device::CreateInstance()
    {
        VkApplicationInfo applicationInfo {};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pApplicationName = "ExplosionEngine";
        applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.pEngineName = "ExplosionEngine";
        applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.apiVersion = VK_API_VERSION_1_2;

        VkInstanceCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &applicationInfo;
        createInfo.enabledExtensionCount = extensions.size();
        createInfo.ppEnabledExtensionNames = extensions.data();
        createInfo.enabledLayerCount = 0;

        if (vkCreateInstance(&createInfo, nullptr, &vkInstance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vulkan instance");
        }
    }

    void Device::DestroyInstance()
    {
        vkDestroyInstance(vkInstance, nullptr);
    }
}
