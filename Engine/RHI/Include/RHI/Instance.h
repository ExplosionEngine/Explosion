//
// Created by johnk on 27/12/2021.
//

#ifndef EXPLOSION_RHI_INSTANCE_H
#define EXPLOSION_RHI_INSTANCE_H

#include <Common/Utility.h>

namespace RHI {
    class PhysicalDevice;

    struct InstanceCreateInfo {
        bool debugMode;
    };

    class Instance {
    public:
        static Instance* CreateByPlatform(const InstanceCreateInfo& info);

        NON_COPYABLE(Instance)
        virtual ~Instance();

        virtual uint32_t CountPhysicalDevices() = 0;
        virtual PhysicalDevice* GetPhysicalDevice(uint32_t idx) = 0;

    protected:
        explicit Instance(const InstanceCreateInfo& info);
    };
}

using RHICreateInstanceFunc = RHI::Instance*(*)(const RHI::InstanceCreateInfo&);

#endif //EXPLOSION_RHI_INSTANCE_H
