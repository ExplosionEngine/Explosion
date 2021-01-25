//
// Created by John Kindem on 2021/1/25 0025.
//

#ifndef EXPLOSION_RENDER_PASS_H
#define EXPLOSION_RENDER_PASS_H

namespace Explosion {
    class RenderPass {
    public:
        struct Descriptor {};

        virtual ~RenderPass() = 0;

    protected:
        RenderPass();
    };
}

#endif //EXPLOSION_RENDER_PASS_H
