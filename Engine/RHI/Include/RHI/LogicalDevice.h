//
// Created by johnk on 31/12/2021.
//

#ifndef EXPLOSION_RHI_LOGICAL_DEVICE_H
#define EXPLOSION_RHI_LOGICAL_DEVICE_H

#include <Common/Utility.h>
#include <RHI/Enum.h>

namespace RHI {
    class Queue;
    struct QueueFamilyCreateInfo;

    struct LogicalDeviceCreateInfo {
        size_t queueFamilyNum;
        const QueueFamilyCreateInfo* queueFamilyCreateInfos;
    };

    class LogicalDevice {
    public:
        NON_COPYABLE(LogicalDevice)
        virtual ~LogicalDevice();

        virtual size_t GetQueueNum(QueueFamilyType familyType) = 0;
        virtual Queue* GetCommandQueue(QueueFamilyType familyType, size_t idx) = 0;

    protected:
        explicit LogicalDevice(const LogicalDeviceCreateInfo* createInfo);
    };
}

#endif //EXPLOSION_RHI_LOGICAL_DEVICE_H
