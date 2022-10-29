//
// Created by Zach Lee on 2022/6/4.
//

#include <RHI/Vulkan/CommandBuffer.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/CommandEncoder.h>
#include <Common/Debug.h>

namespace RHI::Vulkan {

    VKCommandBuffer::VKCommandBuffer(VKDevice& dev, vk::CommandPool p)
        : device(dev)
        , pool(p)
    {
        CreateNativeCommandBuffer();
    }

    VKCommandBuffer::~VKCommandBuffer()
    {
        auto vkDevice = device.GetVkDevice();
        if (commandBuffer) {
            vkDevice.freeCommandBuffers(pool, 1, &commandBuffer);
        }
        for (auto& semaphore : signalSemaphores) {
            vkDevice.destroySemaphore(semaphore);
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

        vk::CommandBufferBeginInfo beginInfo = {};
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        (void) commandBuffer.begin(&beginInfo);
        return new VKCommandEncoder(device, *this);
    }

    vk::CommandBuffer VKCommandBuffer::GetVkCommandBuffer() const
    {
        return commandBuffer;
    }

    void VKCommandBuffer::AddWaitSemaphore(vk::Semaphore semaphore, vk::PipelineStageFlags stage)
    {
        waitSemaphores.emplace_back(semaphore);
        waitStages.emplace_back(stage);
    }

    const std::vector<vk::Semaphore>& VKCommandBuffer::GetWaitSemaphores() const
    {
        return waitSemaphores;
    }

    const std::vector<vk::Semaphore>& VKCommandBuffer::GetSignalSemaphores() const
    {
        return signalSemaphores;
    }

    const std::vector<vk::PipelineStageFlags>& VKCommandBuffer::GetWaitStages() const
    {
        return waitStages;
    }

    void VKCommandBuffer::CreateNativeCommandBuffer()
    {
        vk::CommandBufferAllocateInfo cmdInfo;
        cmdInfo.setCommandBufferCount(1)
            .setCommandPool(pool)
            .setLevel(vk::CommandBufferLevel::ePrimary);

        Assert(device.GetVkDevice().allocateCommandBuffers(&cmdInfo, &commandBuffer) == vk::Result::eSuccess);

        signalSemaphores.resize(1); // TODO
        signalSemaphores[0] = device.GetVkDevice().createSemaphore({}, nullptr);
    }

}