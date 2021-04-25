//
// Created by Administrator on 2021/4/3 0003.
//

#ifndef EXPLOSION_RENDERER_H
#define EXPLOSION_RENDERER_H

#include <cstdint>
#include <vector>

namespace Explosion {
    class Driver;
    class SwapChain;
    class Image;
    class ImageView;
    class RenderPass;
    class FrameBuffer;

    class Renderer {
    public:
        Renderer(Driver& driver, void* surface, uint32_t width, uint32_t height);
        ~Renderer();

        void CreateSwapChain();
        void DestroySwapChain();

    private:
        Driver& driver;
        void* surface;
        uint32_t width;
        uint32_t height;
        SwapChain* swapChain = nullptr;
        RenderPass* renderPass = nullptr;
        std::vector<ImageView*> colorAttachmentViews;
        std::vector<FrameBuffer*> frameBuffers;
    };
}

#endif //EXPLOSION_RENDERER_H
