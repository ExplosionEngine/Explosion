//
// Created by John Kindem on 2021/3/23.
//

#ifndef EXPLOSION_ENGINE_H
#define EXPLOSION_ENGINE_H

#include <string>

#include <vulkan/vulkan.hpp>

namespace Explosion {
    class World;
    class Renderer;

    class Engine {
    public:
        static Engine* GetInstance();
        ~Engine();

        World* CreateWorld(const std::string& name);
        void DestroyWorld(World* world);

        Renderer* CreateRenderer(void* surface, uint32_t width, uint32_t height);
        void DestroyRenderer(Renderer* renderer);

        const VkInstance& GetVkInstance();
        const VkPhysicalDevice& GetVkPhysicalDevice();
        const VkDevice& GetVkDevice();

    private:
        Engine();

        void PrepareInstanceExtensions();
        void CreateVkInstance();
        void DestroyVkInstance();

        std::vector<const char*> instanceExtensions {};
        VkInstance vkInstance = VK_NULL_HANDLE;
        VkPhysicalDevice vkPhysicalDevice = VK_NULL_HANDLE;
        VkDevice vkDevice = VK_NULL_HANDLE;
    };
}

#endif //EXPLOSION_ENGINE_H
