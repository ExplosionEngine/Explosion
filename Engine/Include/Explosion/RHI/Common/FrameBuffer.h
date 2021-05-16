//
// Created by Administrator on 2021/5/16 0016.
//

#ifndef EXPLOSION_FRAMEBUFFER_H
#define EXPLOSION_FRAMEBUFFER_H

namespace Explosion::RHI {
    class FrameBuffer {
    public:
        virtual ~FrameBuffer();

    protected:
        FrameBuffer();
    };
}

#endif //EXPLOSION_FRAMEBUFFER_H
