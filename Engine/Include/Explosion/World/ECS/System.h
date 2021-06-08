//
// Created by John Kindem on 2021/6/8.
//

#ifndef EXPLOSION_SYSTEM_H
#define EXPLOSION_SYSTEM_H

#include <Explosion/World/ECS/Registry.h>

namespace Explosion::ECS {
    class System {
    public:
        explicit System(Registry& registry);
        ~System();

    private:
        Registry& registry;
    };
}

#endif //EXPLOSION_SYSTEM_H
