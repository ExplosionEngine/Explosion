//
// Created by John Kindem on 2021/4/28.
//

#include <cstring>
#include <stdexcept>

#include <Engine/RHI/Vulkan/VulkanBuffer.h>
#include <Engine/RHI/Vulkan/VulkanDriver.h>
#include <Engine/RHI/Vulkan/VulkanCommandBuffer.h>
#include <Engine/RHI/Vulkan/VulkanAdapater.h>

namespace Explosion::RHI {
    VulkanBuffer::VulkanBuffer(VulkanDriver& driver, Config config)
        : Buffer(config), driver(driver), device(*driver.GetDevice())
    {
        CreateBuffer();
        AllocateMemory();
    }

    VulkanBuffer::~VulkanBuffer()
    {
        FreeMemory();
        DestroyBuffer();
    }

    uint32_t VulkanBuffer::GetSize()
    {
        return config.size;
    }

    const VkBuffer& VulkanBuffer::GetVkBuffer()
    {
        return vkBuffer;
    }

    const VkDeviceMemory& VulkanBuffer::GetVkDeviceMemory()
    {
        return vkDeviceMemory;
    }

    void VulkanBuffer::UpdateData(void* data)
    {
        if (config.memoryProperties & FlagsCast(MemoryPropertyBits::DEVICE_LOCAL)) {
            VulkanBuffer::Config stagingConfig = {
                config.size,
                FlagsCast(BufferUsageBits::TRANSFER_SRC),
                MemoryPropertyBits::HOST_VISIBLE | MemoryPropertyBits::HOST_COHERENT
            };
            auto* stagingBuffer = driver.CreateBuffer(stagingConfig);
            stagingBuffer->UpdateData(data);
            {
                auto* commandBuffer = driver.CreateCommandBuffer();
                commandBuffer->EncodeCommands([&stagingBuffer, this](auto* encoder) -> void {
                    encoder->CopyBuffer(stagingBuffer, this);
                });
                commandBuffer->SubmitNow();
                driver.DestroyCommandBuffer(commandBuffer);
            }
            driver.DestroyBuffer(stagingBuffer);
        } else {
            void* mapped = nullptr;
            vkMapMemory(device.GetVkDevice(), vkDeviceMemory, 0, static_cast<VkDeviceSize>(config.size), 0, &mapped);
            memcpy(mapped, data, static_cast<size_t>(config.size));
            vkUnmapMemory(device.GetVkDevice(), vkDeviceMemory);
        }
    }

    void VulkanBuffer::CreateBuffer()
    {
        VkBufferCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.size = static_cast<VkDeviceSize>(config.size);
        createInfo.usage = VkGetFlags<BufferUsageBits, VkBufferUsageFlagBits>(config.usages);
        createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
        createInfo.usage = VkGetFlags<BufferUsageBits, VkBufferUsageFlagBits>(config.usages);

        if (vkCreateBuffer(device.GetVkDevice(), &createInfo, nullptr, &vkBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vulkan buffer");
        }
    }

    void VulkanBuffer::DestroyBuffer()
    {
        vkDestroyBuffer(device.GetVkDevice(), vkBuffer, nullptr);
    }


    void VulkanBuffer::AllocateMemory()
    {
        VkMemoryRequirements memoryRequirements {};
        vkGetBufferMemoryRequirements(device.GetVkDevice(), vkBuffer, &memoryRequirements);
        std::optional<uint32_t> memType = FindMemoryType(
            device.GetVkPhysicalDeviceMemoryProperties(),
            memoryRequirements.memoryTypeBits,
            VkGetFlags<MemoryPropertyBits, VkMemoryPropertyFlagBits>(config.memoryProperties)
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

    void VulkanBuffer::FreeMemory()
    {
        vkFreeMemory(device.GetVkDevice(), vkDeviceMemory, nullptr);
    }
}
