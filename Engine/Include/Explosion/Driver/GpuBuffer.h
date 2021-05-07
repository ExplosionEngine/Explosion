//
// Created by John Kindem on 2021/4/28.
//

#ifndef EXPLOSION_GPUBUFFER_H
#define EXPLOSION_GPUBUFFER_H

#include <cstdint>

#include <vulkan/vulkan.h>

#include <Explosion/Driver/GpuRes.h>
#include <Explosion/Driver/Common.h>

namespace Explosion {
    class Driver;
    class Device;

    class GpuBuffer : public GpuRes {
    public:
        GpuBuffer(Driver& driver, uint32_t size);
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

        Device& device;
        uint32_t size;
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
        HostVisibleBuffer(Driver& driver, uint32_t size);
        ~HostVisibleBuffer() override;
        void UpdateData(void *data) override;

    protected:
        void SetupBufferCreateInfo(VkBufferCreateInfo &createInfo) override;
        VkMemoryPropertyFlags GetMemoryPropertyFlags() override;
    };


    class DeviceLocalBuffer : public GpuBuffer {
    public:
        DeviceLocalBuffer(Driver& driver, uint32_t size);
        ~DeviceLocalBuffer() override;
        void UpdateData(void *data) override;

    protected:
        void SetupBufferCreateInfo(VkBufferCreateInfo &createInfo) override;
        VkMemoryPropertyFlags GetMemoryPropertyFlags() override;
    };

    class StagingBuffer : public HostVisibleBuffer {
    public:
        StagingBuffer(Driver& driver, uint32_t size);
        ~StagingBuffer() override;

    private:
        void SetupBufferCreateInfo(VkBufferCreateInfo &createInfo) override;
        VkMemoryPropertyFlags GetMemoryPropertyFlags() override;
    };

    class UniformBuffer : public HostVisibleBuffer {
    public:
        UniformBuffer(Driver& driver, uint32_t size);
        ~UniformBuffer() override;

    protected:
        void SetupBufferCreateInfo(VkBufferCreateInfo &createInfo) override;
        VkMemoryPropertyFlags GetMemoryPropertyFlags() override;
    };

    class VertexBuffer : public DeviceLocalBuffer {
    public:
        VertexBuffer(Driver& driver, uint32_t size);
        ~VertexBuffer() override;

    protected:
        void SetupBufferCreateInfo(VkBufferCreateInfo &createInfo) override;
        VkMemoryPropertyFlags GetMemoryPropertyFlags() override;
    };

    class IndexBuffer : public DeviceLocalBuffer {
    public:
        IndexBuffer(Driver& driver, uint32_t size);
        ~IndexBuffer() override;

    private:
        void SetupBufferCreateInfo(VkBufferCreateInfo &createInfo) override;
        VkMemoryPropertyFlags GetMemoryPropertyFlags() override;
    };
}

#endif //EXPLOSION_GPUBUFFER_H
