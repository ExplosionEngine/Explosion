//
// Created by johnk on 30/3/2022.
//

#include "RHI/Synchronous.h"
#include <RHI/Synchronous.h>

namespace RHI {
    Barrier Barrier::Transition(Buffer* buffer, BufferState before, BufferState after)
    {
        Barrier barrier {};
        barrier.type = ResourceType::buffer;
        barrier.buffer.pointer = buffer;
        barrier.buffer.before = before;
        barrier.buffer.after = after;
        return barrier;
    }

    Barrier Barrier::Transition(Texture* texture, TextureState before, TextureState after)
    {
        Barrier barrier {};
        barrier.type = ResourceType::texture;
        barrier.texture.pointer = texture;
        barrier.texture.before = before;
        barrier.texture.after = after;
        return barrier;
    }

    Fence::Fence(Device& device, bool initAsSignaled) {}

    Fence::~Fence() = default;

    Semaphore::Semaphore(Device& device) {}

    Semaphore::~Semaphore() = default;
}
