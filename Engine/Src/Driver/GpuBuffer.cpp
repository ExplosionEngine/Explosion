//
// Created by John Kindem on 2021/4/28.
//

#include <Explosion/Driver/GpuBuffer.h>
#include <Explosion/Driver/Driver.h>
#include <Explosion/Driver/CommandBuffer.h>
#include <Explosion/Driver/CommandEncoder.h>

namespace Explosion {
    GpuBuffer::GpuBuffer(Driver& driver, uint32_t size)
        : driver(driver), device(*driver.GetDevice()), size(size)
    {
        CreateBuffer();
        AllocateMemory();
    }

    GpuBuffer::~GpuBuffer()
    {
        FreeMemory();
        DestroyBuffer();
    }

    uint32_t GpuBuffer::GetSize() const
    {
        return size;
    }

    const VkBuffer& GpuBuffer::GetVkBuffer() const
    {
        return vkBuffer;
    }

    const VkDeviceMemory& GpuBuffer::GetVkDeviceMemory() const
    {
        return vkDeviceMemory;
    }

    void GpuBuffer::SetupBufferCreateInfo(VkBufferCreateInfo& createInfo) {}

    VkMemoryPropertyFlags GpuBuffer::GetMemoryPropertyFlags()
    {
        return 0;
    }

    void GpuBuffer::CreateBuffer()
    {
        VkBufferCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.size = static_cast<VkDeviceSize>(size);
        createInfo.usage = 0;
        createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
        SetupBufferCreateInfo(createInfo);

        if (vkCreateBuffer(device.GetVkDevice(), &createInfo, nullptr, &vkBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vulkan buffer");
        }
    }

    void GpuBuffer::DestroyBuffer()
    {
        vkDestroyBuffer(device.GetVkDevice(), vkBuffer, nullptr);
    }


    void GpuBuffer::AllocateMemory()
    {
        VkMemoryRequirements memoryRequirements {};
        vkGetBufferMemoryRequirements(device.GetVkDevice(), vkBuffer, &memoryRequirements);
        std::optional<uint32_t> memType = FindMemoryType(
            device.GetVkPhysicalDeviceMemoryProperties(),
            memoryRequirements.memoryTypeBits,
            GetMemoryPropertyFlags()
        );
        if (!memType.has_value()) {
            throw std::runtime_error("failed to find suitable memory type");
        }

        VkMemoryAllocateInfo allocateInfo {};
        allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocateInfo.pNext = nullptr;
        allocateInfo.allocationSize = static_cast<VkDeviceSize>(size);
        allocateInfo.memoryTypeIndex = memType.value();

        if (vkAllocateMemory(device.GetVkDevice(), &allocateInfo, nullptr, &vkDeviceMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate vulkan device memory");
        }
        vkBindBufferMemory(device.GetVkDevice(), vkBuffer, vkDeviceMemory, 0);
    }

    void GpuBuffer::FreeMemory()
    {
        vkFreeMemory(device.GetVkDevice(), vkDeviceMemory, nullptr);
    }

    HostVisibleBuffer::HostVisibleBuffer(Driver& driver, uint32_t size) : GpuBuffer(driver, size) {}

    HostVisibleBuffer::~HostVisibleBuffer() = default;

    void HostVisibleBuffer::UpdateData(void* data)
    {
        void* mapped = nullptr;
        vkMapMemory(device.GetVkDevice(), vkDeviceMemory, 0, static_cast<VkDeviceSize>(size), 0, &mapped);
        memcpy(mapped, data, static_cast<size_t>(size));
        vkUnmapMemory(device.GetVkDevice(), vkDeviceMemory);
    }

    void HostVisibleBuffer::SetupBufferCreateInfo(VkBufferCreateInfo& createInfo)
    {
        GpuBuffer::SetupBufferCreateInfo(createInfo);
    }

    VkMemoryPropertyFlags HostVisibleBuffer::GetMemoryPropertyFlags()
    {
        return GpuBuffer::GetMemoryPropertyFlags()
            | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
            | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    }

    DeviceLocalBuffer::DeviceLocalBuffer(Driver& driver, uint32_t size) : GpuBuffer(driver, size) {}

    DeviceLocalBuffer::~DeviceLocalBuffer() = default;

    void DeviceLocalBuffer::SetupBufferCreateInfo(VkBufferCreateInfo& createInfo)
    {
        GpuBuffer::SetupBufferCreateInfo(createInfo);
        createInfo.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    }

    VkMemoryPropertyFlags DeviceLocalBuffer::GetMemoryPropertyFlags()
    {
        return GpuBuffer::GetMemoryPropertyFlags()
            | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    }

    void DeviceLocalBuffer::UpdateData(void* data)
    {
        auto* stagingBuffer = driver.CreateGpuRes<StagingBuffer>(size);
        stagingBuffer->UpdateData(data);
        {
            auto* commandBuffer = driver.CreateGpuRes<CommandBuffer>();
            commandBuffer->EncodeCommands([&stagingBuffer, this](auto* encoder) -> void {
                encoder->CopyBuffer(stagingBuffer, this);
            });
            commandBuffer->SubmitNow();
            driver.DestroyGpuRes<CommandBuffer>(commandBuffer);
        }
        driver.DestroyGpuRes<StagingBuffer>(stagingBuffer);
    }

    StagingBuffer::StagingBuffer(Driver& driver, uint32_t size) : HostVisibleBuffer(driver, size) {}

    StagingBuffer::~StagingBuffer() = default;

    void StagingBuffer::SetupBufferCreateInfo(VkBufferCreateInfo& createInfo)
    {
        HostVisibleBuffer::SetupBufferCreateInfo(createInfo);
        createInfo.usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    }

    VkMemoryPropertyFlags StagingBuffer::GetMemoryPropertyFlags()
    {
        return HostVisibleBuffer::GetMemoryPropertyFlags();
    }

    UniformBuffer::UniformBuffer(Driver& driver, uint32_t size) : HostVisibleBuffer(driver, size) {}

    UniformBuffer::~UniformBuffer() = default;

    void UniformBuffer::SetupBufferCreateInfo(VkBufferCreateInfo& createInfo)
    {
        HostVisibleBuffer::SetupBufferCreateInfo(createInfo);
        createInfo.usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    }

    VkMemoryPropertyFlags UniformBuffer::GetMemoryPropertyFlags()
    {
        return HostVisibleBuffer::GetMemoryPropertyFlags();
    }

    VertexBuffer::VertexBuffer(Driver& driver, uint32_t size) : DeviceLocalBuffer(driver, size) {}

    VertexBuffer::~VertexBuffer() = default;

    void VertexBuffer::SetupBufferCreateInfo(VkBufferCreateInfo& createInfo)
    {
        DeviceLocalBuffer::SetupBufferCreateInfo(createInfo);
        createInfo.usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    }

    VkMemoryPropertyFlags VertexBuffer::GetMemoryPropertyFlags()
    {
        return DeviceLocalBuffer::GetMemoryPropertyFlags();
    }

    IndexBuffer::IndexBuffer(Driver& driver, uint32_t size) : DeviceLocalBuffer(driver, size) {}

    IndexBuffer::~IndexBuffer() = default;

    void IndexBuffer::SetupBufferCreateInfo(VkBufferCreateInfo& createInfo)
    {
        DeviceLocalBuffer::SetupBufferCreateInfo(createInfo);
        createInfo.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    }

    VkMemoryPropertyFlags IndexBuffer::GetMemoryPropertyFlags()
    {
        return DeviceLocalBuffer::GetMemoryPropertyFlags();
    }
}
