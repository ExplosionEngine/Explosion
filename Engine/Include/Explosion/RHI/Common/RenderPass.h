//
// Created by John Kindem on 2021/5/16 0016.
//

#ifndef EXPLOSION_RENDERPASS_H
#define EXPLOSION_RENDERPASS_H

namespace Explosion::RHI {
    class RenderPass {
    public:
        virtual ~RenderPass();

    protected:
        RenderPass();
    };
}

#endif //EXPLOSION_RENDERPASS_H
