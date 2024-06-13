//
// Created by johnk on 30/3/2022.
//

#include <RHI/Synchronous.h>

namespace RHI {
    Barrier Barrier::Transition(Buffer* buffer, const BufferState before, const BufferState after)
    {
        Barrier barrier {};
        barrier.type = ResourceType::buffer;
        barrier.buffer.pointer = buffer;
        barrier.buffer.before = before;
        barrier.buffer.after = after;
        return barrier;
    }

    Barrier Barrier::Transition(Texture* texture, const TextureState before, const TextureState after)
    {
        Barrier barrier {};
        barrier.type = ResourceType::texture;
        barrier.texture.pointer = texture;
        barrier.texture.before = before;
        barrier.texture.after = after;
        return barrier;
    }

    Fence::Fence(Device&, bool) {}

    Fence::~Fence() = default;

    Semaphore::Semaphore(Device&) {}

    Semaphore::~Semaphore() = default;
}
