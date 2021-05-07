//
// Created by Administrator on 2021/4/3 0003.
//

#include <Explosion/Core/Renderer.h>
#include <Explosion/Driver/VkAdapater.h>
#include <Explosion/Driver/Driver.h>
#include <Explosion/Driver/Signal.h>

namespace Explosion {
    Renderer::Renderer(Driver& driver, void* surface, uint32_t width, uint32_t height)
        : driver(driver), surface(surface), width(width), height(height) {}

    Renderer::~Renderer() {}
}
