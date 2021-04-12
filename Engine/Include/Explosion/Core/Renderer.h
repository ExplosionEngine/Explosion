//
// Created by Administrator on 2021/4/3 0003.
//

#ifndef EXPLOSION_RENDERER_H
#define EXPLOSION_RENDERER_H

#include <cstdint>

namespace Explosion {
    class Driver;
    class SwapChain;

    class Renderer {
    public:
        Renderer(Driver& driver, void* surface, uint32_t width, uint32_t height);
        ~Renderer();

    private:
        Driver& driver;
        void* surface;
        uint32_t width;
        uint32_t height;
        SwapChain* swapChain = nullptr;
    };
}

#endif //EXPLOSION_RENDERER_H
