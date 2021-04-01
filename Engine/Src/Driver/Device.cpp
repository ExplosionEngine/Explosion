//
// Created by John Kindem on 2021/3/30.
//

#include <Explosion/Driver/Device.h>

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
        // TODO
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

        // TODO
    }

    void Device::DestroyInstance()
    {
        // TODO
    }
}
