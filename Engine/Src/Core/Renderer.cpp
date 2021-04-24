//
// Created by Administrator on 2021/4/3 0003.
//

#include <Explosion/Core/Renderer.h>
#include <Explosion/Driver/Driver.h>
#include <Explosion/Driver/SwapChain.h>

namespace Explosion {
    Renderer::Renderer(Driver& driver, void* surface, uint32_t width, uint32_t height)
        : driver(driver), surface(surface), width(width), height(height)
    {
        CreateSwapChain();
    }

    Renderer::~Renderer()
    {
        DestroySwapChain();
    }

    void Renderer::CreateSwapChain()
    {
        swapChain = driver.CreateGpuRes<SwapChain>(surface, width, height);
    }

    void Renderer::DestroySwapChain()
    {
        driver.DestroyGpuRes<SwapChain>(swapChain);
    }
}
