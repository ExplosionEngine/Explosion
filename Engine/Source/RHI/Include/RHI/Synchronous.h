//
// Created by johnk on 30/3/2022.
//

#pragma once

#include <Common/Utility.h>
#include <RHI/Common.h>
#include <RHI/Device.h>

namespace RHI {
    class Buffer;
    class Texture;

    struct BufferTransitionBase {
        BufferState before;
        BufferState after;
    };

    struct TextureTransitionBase {
        TextureState before;
        TextureState after;
    };

    struct BufferTransition : public BufferTransitionBase {
        Buffer* pointer;
    };

    struct TextureTransition : public TextureTransitionBase {
        Texture* pointer;
    };

    struct Barrier {
    public:
        ~Barrier() = default;

        static Barrier Transition(Buffer* buffer, BufferState before, BufferState after);
        static Barrier Transition(Texture* texture, TextureState before, TextureState after);

        ResourceType type;
        union {
            BufferTransition buffer;
            TextureTransition texture;
        };
    };

    enum class FenceStatus {
        signaled,
        notReady,
        max
    };

    class Fence {
    public:
        NonCopyable(Fence)
        virtual ~Fence();

        virtual FenceStatus GetStatus() = 0;
        virtual void Reset() = 0;
        virtual void Wait() = 0;
        virtual void Destroy() = 0;

    protected:
        explicit Fence(Device& device);
    };
}
