//
// Created by johnk on 16/1/2022.
//

#pragma once

#include <vulkan/vulkan.hpp>

#include <RHI/Queue.h>

namespace RHI::Vulkan {
    class VKQueue : public Queue {
    public:
        NON_COPYABLE(VKQueue)
        explicit VKQueue(vk::Queue vkQueue);
        ~VKQueue() override;

        void Submit(CommandBuffer* commandBuffer) override;

        vk::Queue GetVkQueue();

    private:
        vk::Queue vkQueue;
    };
}
