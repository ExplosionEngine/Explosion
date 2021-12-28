//
// Created by johnk on 27/12/2021.
//

#ifndef EXPLOSION_RHI_DEVICE_H
#define EXPLOSION_RHI_DEVICE_H

#include <Common/Utility.h>

namespace RHI {
    class Instance {
    public:
        NON_COPYABLE(Instance)

    private:
    };

    Instance* CreatePlatformRHI();
}

#endif //EXPLOSION_RHI_DEVICE_H
