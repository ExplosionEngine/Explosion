//
// Created by Administrator on 2021/4/3 0003.
//

#ifndef EXPLOSION_RENDERER_H
#define EXPLOSION_RENDERER_H

#include <cstdint>

namespace Explosion {
    class Engine;
    class Scene;

    class Renderer {
    public:
        Renderer(Engine& engine, void* surface, uint32_t width, uint32_t height);
        ~Renderer();
        void DrawFrame(Scene& scene);

    protected:
        Engine& engine;
        void* surface;
        uint32_t width;
        uint32_t height;
    };
}

#endif //EXPLOSION_RENDERER_H
