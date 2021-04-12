//
// Created by Administrator on 2021/4/3 0003.
//

#include <Explosion/Core/Render/Renderer.h>

namespace Explosion {
    Renderer::Renderer(Engine& engine, void* surface, uint32_t width, uint32_t height)
        : engine(engine), surface(surface), width(width), height(height) {}

    Renderer::~Renderer() = default;

    void Renderer::DrawFrame(Scene& scene)
    {
        // TODO
    }
}
