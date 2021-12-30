//
// Created by johnk on 30/12/2021.
//

#ifndef EXPLOSION_RHI_PHYSICAL_DEVICE_H
#define EXPLOSION_RHI_PHYSICAL_DEVICE_H

#include <Common/Utility.h>

namespace RHI {
    class PhysicalDevice {
    public:
        NON_COPYABLE(PhysicalDevice)
        virtual ~PhysicalDevice();

    protected:
        PhysicalDevice();
    };
}

#endif //EXPLOSION_RHI_PHYSICAL_DEVICE_H
