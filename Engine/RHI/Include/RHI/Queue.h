//
// Created by johnk on 1/1/2022.
//

#ifndef EXPLOSION_RHI_COMMAND_QUEUE_H
#define EXPLOSION_RHI_COMMAND_QUEUE_H

#include <RHI/Enum.h>
#include <Common/Utility.h>

namespace RHI {
    struct QueueFamilyCreateInfo {
        QueueFamilyType type;
        size_t queueNum;
    };

    struct QueueCreateInfo {
        QueueFamilyType type;
    };

    class Queue {
    public:
        NON_COPYABLE(Queue)
        virtual ~Queue();

    protected:
        explicit Queue(const QueueCreateInfo* createInfo);
    };
}

#endif //EXPLOSION_RHI_COMMAND_QUEUE_H
