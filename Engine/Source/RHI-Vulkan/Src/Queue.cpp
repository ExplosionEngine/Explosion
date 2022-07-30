//
// Created by johnk on 16/1/2022.
//

#include <RHI/Vulkan/Queue.h>

namespace RHI::Vulkan {
    VKQueue::VKQueue(vk::Queue q) : Queue(), vkQueue(q) {}

    VKQueue::~VKQueue() = default;

    void VKQueue::Submit(CommandBuffer* commandBuffer, Fence* fenceToSignal)
    {
        // TODO
    }

    void VKQueue::Wait(Fence* fenceToSignal)
    {
        // TODO
    }

    vk::Queue VKQueue::GetVkQueue()
    {
        return vkQueue;
    }
}
