//
// Created by John Kindem on 2021/5/16 0016.
//

#ifndef EXPLOSION_DEVICE_H
#define EXPLOSION_DEVICE_H

#include <RHI/DeviceInfo.h>

namespace Explosion::RHI {

    class Device {
    public:
        virtual ~Device();

        virtual const DeviceInfo& GetDeviceInfo() const = 0;

    protected:
        Device();
    };
}

#endif //EXPLOSION_DEVICE_H
