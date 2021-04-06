//
// Created by John Kindem on 2021/3/30.
//

#ifndef EXPLOSION_DRIVER_H
#define EXPLOSION_DRIVER_H

#include <memory>

#include <Explosion/Driver/Device.h>

namespace Explosion {
    class Driver {
    public:
        Driver();
        ~Driver();

    private:
        std::unique_ptr<Device> device;
    };
}

#endif //EXPLOSION_DRIVER_Hw
