//
// Created by johnk on 16/1/2022.
//

#pragma once

#include <vulkan/vulkan.hpp>

#include <RHI/Queue.h>
#include <RHI/Synchronous.h>

namespace RHI::Vulkan {
    class VKQueue : public Queue {
    public:
        NON_COPYABLE(VKQueue)
        explicit VKQueue(vk::Queue vkQueue);
        ~VKQueue() override;

        void Submit(CommandBuffer* commandBuffer, Fence* fenceToSignal) override;
        void Wait(Fence* fenceToSignal) override;

        vk::Queue GetVkQueue();

    private:
        vk::Queue vkQueue;
    };
}
