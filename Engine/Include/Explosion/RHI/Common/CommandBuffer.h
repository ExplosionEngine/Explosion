//
// Created by John Kindem on 2021/5/16 0016.
//

#ifndef EXPLOSION_COMMANDBUFFER_H
#define EXPLOSION_COMMANDBUFFER_H

namespace Explosion::RHI {
    class CommandBuffer {
    public:
        virtual ~CommandBuffer();

    protected:
        CommandBuffer();
    };
}

#endif //EXPLOSION_COMMANDBUFFER_H
