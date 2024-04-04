//
// Created by johnk on 15/1/2022.
//

#include <RHI/Device.h>

namespace RHI {
    QueueRequestInfo::QueueRequestInfo(QueueType inType, uint8_t inNum)
        : type(inType)
        , num(inNum)
    {
    }

    DeviceCreateInfo::DeviceCreateInfo() = default;

    DeviceCreateInfo& DeviceCreateInfo::AddQueueRequest(const QueueRequestInfo& inQueue)
    {
        queueRequests.emplace_back(inQueue);
        return *this;
    }

    Device::Device(const DeviceCreateInfo& createInfo) {}

    Device::~Device() = default;
}
