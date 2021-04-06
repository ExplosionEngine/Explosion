//
// Created by John Kindem on 2021/3/30.
//

#ifndef EXPLOSION_DRIVER_H
#define EXPLOSION_DRIVER_H

#include <memory>

namespace Explosion {
    class Device;
    class SwapChain;

    class Driver {
    public:
        Driver();
        ~Driver();
        SwapChain* CreateSwapChain(void* surface, uint32_t width, uint32_t height);
        void DestroySwapChain(SwapChain* swapChain);

    private:
        std::unique_ptr<Device> device;
    };
}

#endif //EXPLOSION_DRIVER_Hw
