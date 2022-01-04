//
// Created by johnk on 3/1/2022.
//

#ifndef EXPLOSION_RHI_DEVICE_MEMORY_H
#define EXPLOSION_RHI_DEVICE_MEMORY_H

#include <Common/Utility.h>
#include <RHI/Enum.h>

namespace RHI {
    struct DeviceMemoryAllocateInfo {
        DeviceMemoryType type;
        size_t size;
    };

    class DeviceMemory {
    public:
        NON_COPYABLE(DeviceMemory)
        virtual ~DeviceMemory();

    protected:
        DeviceMemory();
        explicit DeviceMemory(const DeviceMemoryAllocateInfo* allocateInfo);
    };
}

#endif //EXPLOSION_RHI_DEVICE_MEMORY_H
