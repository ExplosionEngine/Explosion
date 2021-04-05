//
// Created by Administrator on 2021/4/3 0003.
//

#include <Explosion/Core/Renderer.h>

namespace Explosion {
    Renderer::Renderer(Driver& driver, void* surface, uint32_t width, uint32_t height)
        : driver(driver), surface(surface), width(width), height(height) {}

    Renderer::~Renderer() = default;
}
