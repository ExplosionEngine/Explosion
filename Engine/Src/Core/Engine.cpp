//
// Created by John Kindem on 2021/3/23.
//

#include <Explosion/Core/Engine.h>
#include <Explosion/Core/World.h>
#include <Explosion/Core/Render/Renderer.h>
#include <Explosion/Core/Platform.h>
#include <Explosion/Common/VkUtils.h>

namespace Explosion {
    Engine::Engine()
    {
        PrepareInstanceExtensions();
        CreateVkInstance();
    }

    Engine::~Engine()
    {
        DestroyVkInstance();
    }

    Engine* Engine::GetInstance()
    {
        static auto* instance = new Engine();
        return instance;
    }

    World* Engine::CreateWorld(const std::string& name)
    {
        return new World(name);
    }

    void Engine::DestroyWorld(World* world)
    {
        delete world;
    }

    Renderer* Engine::CreateRenderer(void* surface, uint32_t width, uint32_t height)
    {
        return new Renderer(*this, surface, width, height);
    }

    void Engine::DestroyRenderer(Renderer* renderer)
    {
        delete renderer;
    }

    const VkInstance& Engine::GetVkInstance()
    {
        return vkInstance;
    }

    const VkPhysicalDevice& Engine::GetVkPhysicalDevice()
    {
        return vkPhysicalDevice;
    }

    const VkDevice& Engine::GetVkDevice()
    {
        return vkDevice;
    }

    void Engine::PrepareInstanceExtensions()
    {
        instanceExtensions.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
        uint32_t platformExtensionNum = GetPlatformInstanceExtensionNum();
        const auto& platformExtensions = GetPlatformInstanceExtensions();
        instanceExtensions.insert(instanceExtensions.end(), platformExtensions, platformExtensions + platformExtensionNum);

        uint32_t propertyCnt = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &propertyCnt, nullptr);
        std::vector<VkExtensionProperties> properties(propertyCnt);
        vkEnumerateInstanceExtensionProperties(nullptr, &propertyCnt, properties.data());
        if (!CheckPropertySupport<VkExtensionProperties>(instanceExtensions, properties, [](const auto* a, const auto& b) -> bool { return std::string(a) == b.extensionName; })) {
            throw std::runtime_error("there are some instance extensions which ware not supported in this device");
        }
    }

    void Engine::CreateVkInstance()
    {
        VkApplicationInfo applicationInfo {};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pApplicationName = "ExplosionEngine";
        applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.pEngineName = "ExplosionEngine";
        applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo instanceCreateInfo {};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pApplicationInfo = &applicationInfo;
        instanceCreateInfo.enabledExtensionCount = instanceExtensions.size();
        instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();
        instanceCreateInfo.enabledLayerCount = 0;
        instanceCreateInfo.ppEnabledLayerNames = nullptr;
        if (vkCreateInstance(&instanceCreateInfo, nullptr, &vkInstance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vulkan instance");
        }
    }

    void Engine::DestroyVkInstance()
    {
        vkDestroyInstance(vkInstance, nullptr);
    }
}
