//
// Created by Zach Lee on 2022/6/4.
//

#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include <RHI/CommandBuffer.h>

namespace RHI::Vulkan {

    class VKDevice;

    class VKCommandBuffer : public CommandBuffer {
    public:
        NON_COPYABLE(VKCommandBuffer)
        VKCommandBuffer(VKDevice& device, VkCommandPool);
        ~VKCommandBuffer() override;

        void Destroy() override;

        CommandEncoder* Begin() override;

        VkCommandBuffer GetVkCommandBuffer() const;

        void AddWaitSemaphore(VkSemaphore semaphore, VkPipelineStageFlags stage);

        const std::vector<VkSemaphore>& GetSignalSemaphores() const;

        const std::vector<VkSemaphore>& GetWaitSemaphores() const;

        const std::vector<VkPipelineStageFlags>& GetWaitStages() const;
    private:
        void CreateNativeCommandBuffer();

        VKDevice& device;
        VkCommandPool pool;
        VkCommandBuffer commandBuffer;
        std::vector<VkSemaphore> signalSemaphores;
        std::vector<VkSemaphore> waitSemaphores;
        std::vector<VkPipelineStageFlags> waitStages;
    };

}
