//
// Created by John Kindem on 2021/4/28.
//

#ifndef EXPLOSION_GPUBUFFER_H
#define EXPLOSION_GPUBUFFER_H

#include <cstdint>

#include <vulkan/vulkan.h>

#include <Explosion/RHI/Enum.h>
#include <Explosion/RHI/Common.h>

namespace Explosion {
    class Driver;
    class Device;

    class GpuBuffer {
    public:
        struct Config {
            uint32_t size;
            std::vector<BufferUsage> usages;
            std::vector<MemoryProperty> memoryProperties;
        };

        GpuBuffer(Driver& driver, Config config);
        ~GpuBuffer();
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

#endif //EXPLOSION_GPUBUFFER_H
