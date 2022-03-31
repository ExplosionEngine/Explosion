//
// Created by johnk on 30/3/2022.
//

#pragma once

#include <Common/Utility.h>
#include <RHI/Enum.h>

namespace RHI {
    class Buffer;
    class Texture;

    struct Barrier {
    public:
        ~Barrier() = default;

        static Barrier Transition(Buffer* buffer, BufferState before, BufferState after);
        static Barrier Transition(Texture* texture, TextureState before, TextureState after);

    public:
        struct BufferTransition {
            Buffer* pointer;
            BufferState before;
            BufferState after;
        };

        struct TextureTransition {
            Texture* pointer;
            TextureState before;
            TextureState after;
        };

        ResourceType type;
        union {
            BufferTransition buffer;
            TextureTransition texture;
        };
    };
}
