//
// Created by John Kindem on 2021/5/16 0016.
//

#ifndef EXPLOSION_FRAMEBUFFER_H
#define EXPLOSION_FRAMEBUFFER_H

#include <cstdint>
#include <vector>

namespace Explosion::RHI {
    class RenderPass;
    class ImageView;

    class FrameBuffer {
    public:
        struct Config {
            RenderPass* renderPass;
            uint32_t width;
            uint32_t height;
            uint32_t layers;
            std::vector<ImageView*> attachments;
        };

        virtual ~FrameBuffer();

    protected:
        explicit FrameBuffer(Config config);

        Config config;
    };
}

#endif //EXPLOSION_FRAMEBUFFER_H
