//
// Created by johnk on 16/1/2022.
//

#include <RHI/Vulkan/Queue.h>

namespace RHI::Vulkan {
    VKQueue::VKQueue(vk::Queue q) : Queue(), vkQueue(q) {}

    VKQueue::~VKQueue() = default;

    vk::Queue VKQueue::GetVkQueue()
    {
        return vkQueue;
    }
}
