//
// Created by johnk on 27/12/2021.
//

#ifndef EXPLOSION_RHI_INSTANCE_H
#define EXPLOSION_RHI_INSTANCE_H

#include <Common/Utility.h>

namespace RHI {
    class Instance {
    public:
        static Instance* CreateByPlatform();

        NON_COPYABLE(Instance)
        virtual ~Instance() = 0;

    protected:
        Instance();
    };
}

using RHICreateInstanceFunc = RHI::Instance*(*)();

#endif //EXPLOSION_RHI_INSTANCE_H
