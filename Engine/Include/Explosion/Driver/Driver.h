//
// Created by John Kindem on 2021/3/30.
//

#ifndef EXPLOSION_DRIVER_H
#define EXPLOSION_DRIVER_H

#include <memory>

#include <Explosion/Driver/Device.h>
#include <Explosion/Driver/SwapChain.h>
#include <Explosion/Driver/Image.h>

namespace Explosion {
    class Driver {
    public:
        Driver();
        ~Driver();
        SwapChain* CreateSwapChain(void* surface, uint32_t width, uint32_t height);
        void DestroySwapChain(SwapChain* swapChain);
        Image* CreateSwapChainImage(SwapChain* swapChain, uint32_t imageIndex);
        void DestroyImage(Image* image);

    private:
        std::unique_ptr<Device> device;
    };
}

#endif //EXPLOSION_DRIVER_Hw
