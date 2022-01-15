//
// Created by johnk on 15/1/2022.
//

#ifndef EXPLOSION_RHI_QUEUE_H
#define EXPLOSION_RHI_QUEUE_H

#include <Common/Utility.h>

namespace RHI {
    class Queue {
    public:
        NON_COPYABLE(Queue)
        virtual ~Queue();

    private:
        Queue();
    };
}

#endif //EXPLOSION_RHI_QUEUE_H
