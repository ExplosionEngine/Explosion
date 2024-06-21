//
// Created by Zach Lee on 2022/6/4.
//

#include <RHI/Vulkan/CommandBuffer.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/CommandRecorder.h>
#include <Common/Debug.h>

namespace RHI::Vulkan {
    VulkanCommandBuffer::VulkanCommandBuffer(VulkanDevice& inDevice, VkCommandPool inNativeCmdPool)
        : device(inDevice)
        , pool(inNativeCmdPool)
    {
        CreateNativeCommandBuffer();
    }

    VulkanCommandBuffer::~VulkanCommandBuffer()
    {
        auto vkDevice = device.GetNative();
        if (nativeCmdBuffer != VK_NULL_HANDLE) {
            vkFreeCommandBuffers(vkDevice, pool, 1, &nativeCmdBuffer);
        }
    }

    Common::UniqueRef<CommandRecorder> VulkanCommandBuffer::Begin()
    {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        // TODO maybe expose this to create info ?
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

        vkBeginCommandBuffer(nativeCmdBuffer, &beginInfo);
        return Common::UniqueRef<CommandRecorder>(new VulkanCommandRecorder(device, *this));
    }

    VkCommandBuffer VulkanCommandBuffer::GetNative() const
    {
        return nativeCmdBuffer;
    }

    void VulkanCommandBuffer::CreateNativeCommandBuffer()
    {
        VkCommandBufferAllocateInfo cmdInfo = {};
        cmdInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cmdInfo.commandBufferCount = 1;
        cmdInfo.commandPool = pool;
        cmdInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

        Assert(vkAllocateCommandBuffers(device.GetNative(), &cmdInfo, &nativeCmdBuffer) == VK_SUCCESS);
    }
}