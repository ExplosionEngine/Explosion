//
// Created by Zach Lee on 2022/6/4.
//

#pragma once

#include <vulkan/vulkan.hpp>
#include <RHI/CommandBuffer.h>

namespace RHI::Vulkan {

    class VKDevice;

    class VKCommandBuffer : public CommandBuffer {
    public:
        NON_COPYABLE(VKCommandBuffer)
        VKCommandBuffer(VKDevice& device, vk::CommandPool);
        ~VKCommandBuffer() override;

        void Destroy() override;

        CommandEncoder* Begin() override;

        vk::CommandBuffer GetVkCommandBuffer() const;

        void AddWaitSemaphore(vk::Semaphore semaphore, vk::PipelineStageFlags stage);

        const std::vector<vk::Semaphore>& GetSignalSemaphores() const;

        const std::vector<vk::Semaphore>& GetWaitSemaphores() const;

        const std::vector<vk::PipelineStageFlags>& GetWaitStages() const;
    private:
        void CreateNativeCommandBuffer();

        VKDevice& device;
        vk::CommandPool pool;
        vk::CommandBuffer commandBuffer;
        std::vector<vk::Semaphore> signalSemaphores;
        std::vector<vk::Semaphore> waitSemaphores;
        std::vector<vk::PipelineStageFlags> waitStages;
    };

}
