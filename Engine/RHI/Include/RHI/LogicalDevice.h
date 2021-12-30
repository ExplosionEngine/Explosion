//
// Created by johnk on 31/12/2021.
//

#ifndef EXPLOSION_RHI_LOGICAL_DEVICE_H
#define EXPLOSION_RHI_LOGICAL_DEVICE_H

#include <Common/Utility.h>

namespace RHI {
    class LogicalDevice {
    public:
        NON_COPYABLE(LogicalDevice)
        virtual ~LogicalDevice();

    protected:
        LogicalDevice();
    };
}

#endif //EXPLOSION_RHI_LOGICAL_DEVICE_H
