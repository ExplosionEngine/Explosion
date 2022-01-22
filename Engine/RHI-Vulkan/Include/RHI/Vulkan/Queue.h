//
// Created by johnk on 16/1/2022.
//

#ifndef EXPLOSION_RHI_VULKAN_QUEUE_H
#define EXPLOSION_RHI_VULKAN_QUEUE_H

#include <vulkan/vulkan.hpp>

#include <RHI/Queue.h>

namespace RHI::Vulkan {
    class VKQueue : public Queue {
    public:
        NON_COPYABLE(VKQueue)
        explicit VKQueue(vk::Queue vkQueue);
        ~VKQueue() override;

        vk::Queue GetVkQueue();

    private:
        vk::Queue vkQueue;
    };
}

#endif //EXPLOSION_RHI_VULKAN_QUEUE_H
