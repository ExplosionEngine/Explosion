//
// Created by John Kindem on 2021/3/30.
//

#include <Explosion/RHI/Driver.h>

namespace Explosion {
    Driver::Driver() : device(std::make_unique<Device>(*this)) {}

    Driver::~Driver() = default;

    Device* Driver::GetDevice()
    {
        return device.get();
    }
}
