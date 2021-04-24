//
// Created by John Kindem on 2021/3/30.
//

#include <Explosion/Driver/Driver.h>

namespace Explosion {
    Driver::Driver() : device(std::make_unique<Device>()) {}

    Driver::~Driver() = default;

    Device* Driver::GetDevice()
    {
        return device.get();
    }
}
