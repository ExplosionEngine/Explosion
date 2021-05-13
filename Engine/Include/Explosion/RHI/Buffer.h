//
// Created by John Kindem on 2021/4/28.
//

#ifndef EXPLOSION_BUFFER_H
#define EXPLOSION_BUFFER_H

#include <cstdint>

#include <vulkan/vulkan.h>

#include <Explosion/RHI/Enum.h>
#include <Explosion/RHI/Common.h>

namespace Explosion::RHI {
    class Driver;
    class Device;

    class Buffer {
    public:
        struct Config {
            uint32_t size;
            std::vector<BufferUsage> usages;
            std::vector<MemoryProperty> memoryProperties;
        };

        Buffer(Driver& driver, Config config);
        ~Buffer();
        uint32_t GetSize();
        const VkBuffer& GetVkBuffer();
        const VkDeviceMemory& GetVkDeviceMemory();
        void UpdateData(void* data);

    private:
        void DestroyBuffer();
        void CreateBuffer();

        void AllocateMemory();
        void FreeMemory();

        Driver& driver;
        Device& device;
        Config config;
        VkBuffer vkBuffer = VK_NULL_HANDLE;
        VkDeviceMemory vkDeviceMemory = VK_NULL_HANDLE;
        bool isDeviceLocal = false;
    };
}

#endif //EXPLOSION_BUFFER_H
