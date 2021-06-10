//
// Created by John Kindem on 2021/6/3.
//

#ifndef EXPLOSION_WORLD_H
#define EXPLOSION_WORLD_H

#include <Explosion/World/ECS/ECS.h>

namespace Explosion {
    class World {
    public:
        World();
        ~World();

        void Tick(float time);
        void AddSystem(ECS::System system);

    private:
        ECS::Registry registry;
        std::vector<ECS::System> systems;
    };
}

#endif //EXPLOSION_WORLD_H
