//
// Created by Administrator on 2021/4/3 0003.
//

#include <Explosion/Core/Renderer.h>
#include <Explosion/RHI/Vulkan/VulkanDriver.h>
#include <Explosion/RHI/Vulkan/VulkanSignal.h>

namespace Explosion {
    Renderer::Renderer(RHI::VulkanDriver& driver, void* surface, uint32_t width, uint32_t height)
        : driver(driver), surface(surface), width(width), height(height) {}

    Renderer::~Renderer() {}
}
