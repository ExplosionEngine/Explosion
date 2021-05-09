//
// Created by John Kindem on 2021/4/28.
//

#ifndef EXPLOSION_GPUBUFFER_H
#define EXPLOSION_GPUBUFFER_H

#include <cstdint>

#include <vulkan/vulkan.h>

#include <Explosion/Driver/Enum.h>
#include <Explosion/Driver/GpuRes.h>
#include <Explosion/Driver/Common.h>

namespace Explosion {
    class Driver;
    class Device;

    class GpuBuffer : public GpuRes {
    public:
        struct Config {
            uint32_t size;
            std::vector<BufferUsage> usages;
        };

        GpuBuffer(Driver& driver, const Config& config);
        ~GpuBuffer() override;
        uint32_t GetSize() const;
        const VkBuffer& GetVkBuffer() const;
        const VkDeviceMemory& GetVkDeviceMemory() const;
        virtual void UpdateData(void* data) = 0;

    protected:
        void OnCreate() override;
        void OnDestroy() override;
        virtual void SetupBufferCreateInfo(VkBufferCreateInfo& createInfo);
        virtual VkMemoryPropertyFlags GetMemoryPropertyFlags();

        Config config;
        Device& device;
        VkBuffer vkBuffer = VK_NULL_HANDLE;
        VkDeviceMemory vkDeviceMemory = VK_NULL_HANDLE;

    private:
        void DestroyBuffer();
        void CreateBuffer();

        void AllocateMemory();
        void FreeMemory();
    };

    class HostVisibleBuffer : public GpuBuffer {
    public:
        HostVisibleBuffer(Driver& driver, const Config& config);
        ~HostVisibleBuffer() override;
        void UpdateData(void *data) override;

    protected:
        VkMemoryPropertyFlags GetMemoryPropertyFlags() override;
    };

    class DeviceLocalBuffer : public GpuBuffer {
    public:
        DeviceLocalBuffer(Driver& driver, const Config& config);
        ~DeviceLocalBuffer() override;
        void UpdateData(void *data) override;

    protected:
        void SetupBufferCreateInfo(VkBufferCreateInfo& createInfo) override;
        VkMemoryPropertyFlags GetMemoryPropertyFlags() override;
    };
}

#endif //EXPLOSION_GPUBUFFER_H
