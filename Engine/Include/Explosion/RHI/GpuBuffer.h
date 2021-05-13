//
// Created by John Kindem on 2021/4/28.
//

#ifndef EXPLOSION_GPUBUFFER_H
#define EXPLOSION_GPUBUFFER_H

#include <cstdint>

#include <vulkan/vulkan.h>

#include <Explosion/RHI/Enum.h>
#include <Explosion/RHI/GpuRes.h>
#include <Explosion/RHI/Common.h>

namespace Explosion {
    class Driver;
    class Device;

    class GpuBuffer : public GpuRes {
    public:
        struct Config {
            uint32_t size;
            std::vector<BufferUsage> usages;
            std::vector<MemoryProperty> memoryProperties;
        };

        GpuBuffer(Driver& driver, const Config& config);
        ~GpuBuffer() override;
        uint32_t GetSize() const;
        const VkBuffer& GetVkBuffer() const;
        const VkDeviceMemory& GetVkDeviceMemory() const;
        void UpdateData(void* data);

    protected:
        void OnCreate() override;
        void OnDestroy() override;

        Config config;
        Device& device;
        VkBuffer vkBuffer = VK_NULL_HANDLE;
        VkDeviceMemory vkDeviceMemory = VK_NULL_HANDLE;
        bool isDeviceLocal = false;

    private:
        void DestroyBuffer();
        void CreateBuffer();

        void AllocateMemory();
        void FreeMemory();
    };
}

#endif //EXPLOSION_GPUBUFFER_H
