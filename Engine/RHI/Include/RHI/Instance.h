//
// Created by johnk on 9/1/2022.
//

#ifndef EXPLOSION_RHI_INSTANCE_H
#define EXPLOSION_RHI_INSTANCE_H

#include <Common/Utility.h>

namespace RHI {
    struct InstanceCreateInfo {
        bool debugMode;
    };

    class Instance {
    public:
        static Instance* CreateInstanceByPlatform(const InstanceCreateInfo* createInfo);

        NON_COPYABLE(Instance)
        virtual ~Instance();

    protected:
        explicit Instance(const InstanceCreateInfo& createInfo);
    };

    using RHICreateInstanceFunc = Instance*(*)(const InstanceCreateInfo*);
}

#endif //EXPLOSION_RHI_INSTANCE_H
