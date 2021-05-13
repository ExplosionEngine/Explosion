//
// Created by John Kindem on 2021/4/28.
//

#include <cstring>
#include <stdexcept>
#include <utility>

#include <Explosion/RHI/Buffer.h>
#include <Explosion/RHI/Driver.h>
#include <Explosion/RHI/CommandBuffer.h>
#include <Explosion/RHI/VkAdapater.h>

namespace Explosion::RHI {
    Buffer::Buffer(Driver& driver, Config config)
        : driver(driver), device(*driver.GetDevice()), config(std::move(config))
    {
        CreateBuffer();
        AllocateMemory();
    }

    Buffer::~Buffer()
    {
        FreeMemory();
        DestroyBuffer();
    }

    uint32_t Buffer::GetSize()
    {
        return config.size;
    }

    const VkBuffer& Buffer::GetVkBuffer()
    {
        return vkBuffer;
    }

    const VkDeviceMemory& Buffer::GetVkDeviceMemory()
    {
        return vkDeviceMemory;
    }

    void Buffer::UpdateData(void* data)
    {
        if (isDeviceLocal) {
            Buffer::Config stagingConfig = {
                config.size,
                { BufferUsage::TRANSFER_SRC },
                { MemoryProperty::HOST_VISIBLE, MemoryProperty::HOST_COHERENT }
            };
            auto* stagingBuffer = driver.CreateGpuRes<Buffer>(stagingConfig);
            stagingBuffer->UpdateData(data);
            {
                auto* commandBuffer = driver.CreateGpuRes<CommandBuffer>();
                commandBuffer->EncodeCommands([&stagingBuffer, this](auto* encoder) -> void {
                    encoder->CopyBuffer(stagingBuffer, this);
                });
                commandBuffer->SubmitNow();
                driver.DestroyGpuRes<CommandBuffer>(commandBuffer);
            }
            driver.DestroyGpuRes<Buffer>(stagingBuffer);
        } else {
            void* mapped = nullptr;
            vkMapMemory(device.GetVkDevice(), vkDeviceMemory, 0, static_cast<VkDeviceSize>(config.size), 0, &mapped);
            memcpy(mapped, data, static_cast<size_t>(config.size));
            vkUnmapMemory(device.GetVkDevice(), vkDeviceMemory);
        }
    }

    void Buffer::CreateBuffer()
    {
        VkBufferCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.size = static_cast<VkDeviceSize>(config.size);
        createInfo.usage = VkGetFlags<BufferUsage, VkBufferUsageFlagBits>(config.usages);
        createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
        for (auto& usage : config.usages) {
            createInfo.usage |= VkConvert<BufferUsage, VkBufferUsageFlagBits>(usage);
        }

        if (vkCreateBuffer(device.GetVkDevice(), &createInfo, nullptr, &vkBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vulkan buffer");
        }
    }

    void Buffer::DestroyBuffer()
    {
        vkDestroyBuffer(device.GetVkDevice(), vkBuffer, nullptr);
    }


    void Buffer::AllocateMemory()
    {
        for (auto& memoryProperty : config.memoryProperties) {
            if (memoryProperty == MemoryProperty::DEVICE_LOCAL) {
                isDeviceLocal = true;
            }
        }

        VkMemoryRequirements memoryRequirements {};
        vkGetBufferMemoryRequirements(device.GetVkDevice(), vkBuffer, &memoryRequirements);
        std::optional<uint32_t> memType = FindMemoryType(
            device.GetVkPhysicalDeviceMemoryProperties(),
            memoryRequirements.memoryTypeBits,
            VkGetFlags<MemoryProperty, VkMemoryPropertyFlagBits>(config.memoryProperties)
        );
        if (!memType.has_value()) {
            throw std::runtime_error("failed to find suitable memory type");
        }

        VkMemoryAllocateInfo allocateInfo {};
        allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocateInfo.pNext = nullptr;
        allocateInfo.allocationSize = static_cast<VkDeviceSize>(config.size);
        allocateInfo.memoryTypeIndex = memType.value();

        if (vkAllocateMemory(device.GetVkDevice(), &allocateInfo, nullptr, &vkDeviceMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate vulkan device memory");
        }
        vkBindBufferMemory(device.GetVkDevice(), vkBuffer, vkDeviceMemory, 0);
    }

    void Buffer::FreeMemory()
    {
        vkFreeMemory(device.GetVkDevice(), vkDeviceMemory, nullptr);
    }
}
