//
// Created by Zach Lee on 2022/6/4.
//

#include <RHI/Vulkan/CommandBuffer.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/CommandEncoder.h>
#include <Common/Debug.h>

namespace RHI::Vulkan {

    VKCommandBuffer::VKCommandBuffer(VKDevice& dev, VkCommandPool p)
        : device(dev)
        , pool(p)
    {
        CreateNativeCommandBuffer();
    }

    VKCommandBuffer::~VKCommandBuffer()
    {
        auto vkDevice = device.GetVkDevice();
        if (commandBuffer) {
            vkFreeCommandBuffers(vkDevice, pool, 1, &commandBuffer);
        }
        for (auto& semaphore : signalSemaphores) {
            vkDestroySemaphore(vkDevice, semaphore, nullptr);
        }
    }

    void VKCommandBuffer::Destroy()
    {
        delete this;
    }

    CommandEncoder* VKCommandBuffer::Begin()
    {
        waitSemaphores.clear();
        waitStages.clear();

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);
        return new VKCommandEncoder(device, *this);
    }

    VkCommandBuffer VKCommandBuffer::GetVkCommandBuffer() const
    {
        return commandBuffer;
    }

    void VKCommandBuffer::AddWaitSemaphore(VkSemaphore semaphore, VkPipelineStageFlags stage)
    {
        waitSemaphores.emplace_back(semaphore);
        waitStages.emplace_back(stage);
    }

    const std::vector<VkSemaphore>& VKCommandBuffer::GetWaitSemaphores() const
    {
        return waitSemaphores;
    }

    const std::vector<VkSemaphore>& VKCommandBuffer::GetSignalSemaphores() const
    {
        return signalSemaphores;
    }

    const std::vector<VkPipelineStageFlags>& VKCommandBuffer::GetWaitStages() const
    {
        return waitStages;
    }

    void VKCommandBuffer::CreateNativeCommandBuffer()
    {
        VkCommandBufferAllocateInfo cmdInfo;
        cmdInfo.commandBufferCount = 1;
        cmdInfo.commandPool = pool;
        cmdInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

        Assert(vkAllocateCommandBuffers(device.GetVkDevice(), &cmdInfo, &commandBuffer) == VK_SUCCESS);

        signalSemaphores.resize(1); // TODO
        Assert(vkCreateSemaphore(device.GetVkDevice(), {}, nullptr, &signalSemaphores[0]) == VK_SUCCESS);
    }

}