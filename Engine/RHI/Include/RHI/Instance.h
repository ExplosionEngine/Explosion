//
// Created by johnk on 9/1/2022.
//

#ifndef EXPLOSION_RHI_INSTANCE_H
#define EXPLOSION_RHI_INSTANCE_H

#include <Common/Utility.h>

#include <RHI/Enum.h>

namespace RHI {
    class Instance {
    public:
        static Instance* CreateByPlatform();
        static Instance* CreateByType(const RHIType& type);

        NON_COPYABLE(Instance)
        virtual ~Instance();
        virtual RHIType GetRHIType() = 0;

    protected:
        explicit Instance();
    };

    using RHICreateInstanceFunc = Instance*(*)();
}

#endif //EXPLOSION_RHI_INSTANCE_H
