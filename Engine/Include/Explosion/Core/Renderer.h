//
// Created by Administrator on 2021/4/3 0003.
//

#ifndef EXPLOSION_RENDERER_H
#define EXPLOSION_RENDERER_H

#include <cstdint>
#include <vector>

#include <Explosion/RHI/Vulkan/VulkanDriver.h>
#include <Explosion/RHI/Vulkan/VulkanSwapChain.h>

namespace Explosion {
    class Renderer {
    public:
        Renderer(RHI::VulkanDriver& driver, void* surface, uint32_t width, uint32_t height);
        ~Renderer();

    private:
        RHI::VulkanDriver& driver;
        void* surface;
        uint32_t width;
        uint32_t height;
    };
}

#endif //EXPLOSION_RENDERER_H
